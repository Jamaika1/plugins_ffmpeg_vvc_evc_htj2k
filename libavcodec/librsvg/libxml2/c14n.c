/*
 * "Canonical XML" implementation
 * http://www.w3.org/TR/xml-c14n
 *
 * "Exclusive XML Canonicalization" implementation
 * http://www.w3.org/TR/xml-exc-c14n
 *
 * See Copyright for the status of this software.
 *
 * Author: Aleksey Sanin
 */
#define IN_LIBXML
#include "libxml.h"
#ifdef LIBXML_C14N_ENABLED

#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "parser.h"
#include "uri.h"
#include "xmlerror.h"
#include "xpathInternals.h"
#include "c14n.h"

#include "private/error.h"
#include "private/io.h"
#include "private/memory.h"

/************************************************************************
 *									*
 *		Some declaration better left private ATM		*
 *									*
 ************************************************************************/

typedef enum {
    XMLC14N_BEFORE_DOCUMENT_ELEMENT = 0,
    XMLC14N_INSIDE_DOCUMENT_ELEMENT = 1,
    XMLC14N_AFTER_DOCUMENT_ELEMENT = 2
} xmlC14NPosition;

typedef struct _xmlC14NVisibleNsStack {
    int nsCurEnd;           /* number of nodes in the set */
    int nsPrevStart;        /* the beginning of the stack for previous visible node */
    int nsPrevEnd;          /* the end of the stack for previous visible node */
    int nsMax;              /* size of the array as allocated */
    xmlNsPtr	*nsTab;	    /* array of ns in no particular order */
    xmlNodePtr	*nodeTab;   /* array of nodes in no particular order */
} xmlC14NVisibleNsStack, *xmlC14NVisibleNsStackPtr;

typedef struct _xmlC14NCtx {
    /* input parameters */
    xmlDocPtr doc;
    xmlC14NIsVisibleCallback is_visible_callback;
    void* user_data;
    int with_comments;
    xmlOutputBufferPtr buf;

    /* position in the XML document */
    xmlC14NPosition pos;
    int parent_is_doc;
    xmlC14NVisibleNsStackPtr ns_rendered;

    /* C14N mode */
    xmlC14NMode mode;

    /* exclusive canonicalization */
    xmlChar **inclusive_ns_prefixes;

    /* error number */
    int error;
} xmlC14NCtx, *xmlC14NCtxPtr;

static xmlC14NVisibleNsStackPtr	xmlC14NVisibleNsStackCreate	(void);
static void     xmlC14NVisibleNsStackDestroy	(xmlC14NVisibleNsStackPtr cur);
static int      xmlC14NVisibleNsStackAdd	    (xmlC14NVisibleNsStackPtr cur,
                                                 xmlNsPtr ns,
                                                 xmlNodePtr node);
static void			xmlC14NVisibleNsStackSave	(xmlC14NVisibleNsStackPtr cur,
								 xmlC14NVisibleNsStackPtr state);
static void			xmlC14NVisibleNsStackRestore	(xmlC14NVisibleNsStackPtr cur,
								 xmlC14NVisibleNsStackPtr state);
static void			xmlC14NVisibleNsStackShift	(xmlC14NVisibleNsStackPtr cur);
static int			xmlC14NVisibleNsStackFind	(xmlC14NVisibleNsStackPtr cur,
								 xmlNsPtr ns);
static int			xmlExcC14NVisibleNsStackFind	(xmlC14NVisibleNsStackPtr cur,
								 xmlNsPtr ns,
								 xmlC14NCtxPtr ctx);

static int			xmlC14NIsNodeInNodeset		(void *user_data,
								 xmlNodePtr node,
								 xmlNodePtr parent);



static int xmlC14NProcessNode(xmlC14NCtxPtr ctx, xmlNodePtr cur);
static int xmlC14NProcessNodeList(xmlC14NCtxPtr ctx, xmlNodePtr cur);
typedef enum {
    XMLC14N_NORMALIZE_ATTR = 0,
    XMLC14N_NORMALIZE_COMMENT = 1,
    XMLC14N_NORMALIZE_PI = 2,
    XMLC14N_NORMALIZE_TEXT = 3
} xmlC14NNormalizationMode;

static xmlChar *xmlC11NNormalizeString(const xmlChar * input,
                                       xmlC14NNormalizationMode mode);

#define	xmlC11NNormalizeAttr( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_ATTR)
#define	xmlC11NNormalizeComment( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_COMMENT)
#define	xmlC11NNormalizePI( a )	\
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_PI)
#define	xmlC11NNormalizeText( a ) \
    xmlC11NNormalizeString((a), XMLC14N_NORMALIZE_TEXT)

#define	xmlC14NIsVisible( ctx, node, parent ) \
     (((ctx)->is_visible_callback != NULL) ? \
	(ctx)->is_visible_callback((ctx)->user_data, \
		(xmlNodePtr)(node), (xmlNodePtr)(parent)) : 1)

#define	xmlC14NIsExclusive( ctx ) \
    ( (ctx)->mode == XML_C14N_EXCLUSIVE_1_0 )

/************************************************************************
 *									*
 *		Some factorized error routines				*
 *									*
 ************************************************************************/

/**
 * Handle a redefinition of memory error
 *
 * @param ctxt  a C14N evaluation context
 */
static void
xmlC14NErrMemory(xmlC14NCtxPtr ctxt)
{
    if (ctxt != NULL)
        ctxt->error = XML_ERR_NO_MEMORY;

    xmlRaiseMemoryError(NULL, NULL, NULL, XML_FROM_C14N, NULL);
}

static void
xmlC14NErrFull(xmlC14NCtxPtr ctxt, xmlNodePtr node, int code, const char *str1,
               const char *msg, ...)
{
    va_list ap;
    int res;

    if (ctxt != NULL)
        ctxt->error = code;

    va_start(ap, msg);
    res = xmlVRaiseError(NULL, NULL, NULL, ctxt, node,
                         XML_FROM_C14N, code, XML_ERR_ERROR, NULL, 0,
                         str1, NULL, NULL, 0, 0,
                         msg, ap);
    va_end(ap);
    if (res < 0)
        xmlC14NErrMemory(ctxt);
}

/**
 * Handle a param error
 *
 * @param ctxt  a C14N evaluation context
 */
static void
xmlC14NErrParam(xmlC14NCtxPtr ctxt)
{
    xmlC14NErrFull(ctxt, NULL, XML_ERR_ARGUMENT, NULL,
		   "Invalid argument\n", NULL);
}

/**
 * Handle an invalid node error
 *
 * @param ctxt  a C14N evaluation context
 * @param node_type  node type
 * @param extra  extra information
 */
static void
xmlC14NErrInvalidNode(xmlC14NCtxPtr ctxt, const char *node_type,
                      const char *extra)
{
    xmlC14NErrFull(ctxt, NULL, XML_C14N_INVALID_NODE, extra,
		   "Node %s is invalid here : %s\n", node_type, extra);
}

/**
 * Handle an unknown node error
 *
 * @param ctxt  a C14N evaluation context
 * @param node_type  node type
 * @param extra  extra information
 */
static void
xmlC14NErrUnknownNode(xmlC14NCtxPtr ctxt, int node_type, const char *extra)
{
    xmlC14NErrFull(ctxt, NULL, XML_C14N_UNKNOW_NODE, extra,
		   "Unknown node type %d found : %s\n", node_type, extra);
}

/**
 * Handle a relative namespace error
 *
 * @param ctxt  a C14N evaluation context
 * @param ns_uri  namespace URI
 */
static void
xmlC14NErrRelativeNamespace(xmlC14NCtxPtr ctxt, const char *ns_uri)
{
    xmlC14NErrFull(ctxt, NULL, XML_C14N_RELATIVE_NAMESPACE, ns_uri,
		   "Relative namespace UR is invalid here : %s\n", ns_uri);
}



/**
 * Handle an error
 *
 * @param ctxt  a C14N evaluation context
 * @param node  the context node
 * @param error  the error code
 * @param msg  the message
 */
static void
xmlC14NErr(xmlC14NCtxPtr ctxt, xmlNodePtr node, int error,
           const char * msg)
{
    xmlC14NErrFull(ctxt, node, error, NULL, "%s", msg);
}

/************************************************************************
 *									*
 *		The implementation internals				*
 *									*
 ************************************************************************/
#define XML_NAMESPACES_DEFAULT		16

static int
xmlC14NIsNodeInNodeset(void *user_data, xmlNodePtr node, xmlNodePtr parent) {
    xmlNodeSetPtr nodes = (xmlNodeSetPtr) user_data;
    if((nodes != NULL) && (node != NULL)) {
	if(node->type != XML_NAMESPACE_DECL) {
	    return(xmlXPathNodeSetContains(nodes, node));
	} else {
	    xmlNs ns;

	    memcpy(&ns, node, sizeof(ns));

	    /* this is a libxml hack! check xpath.c for details */
	    if((parent != NULL) && (parent->type == XML_ATTRIBUTE_NODE)) {
		ns.next = (xmlNsPtr)parent->parent;
	    } else {
		ns.next = (xmlNsPtr)parent;
	    }

	    /*
	     * If the input is an XPath node-set, then the node-set must explicitly
	     * contain every node to be rendered to the canonical form.
	     */
	    return(xmlXPathNodeSetContains(nodes, (xmlNodePtr)&ns));
	}
    }
    return(1);
}

static xmlC14NVisibleNsStackPtr
xmlC14NVisibleNsStackCreate(void) {
    xmlC14NVisibleNsStackPtr ret;

    ret = (xmlC14NVisibleNsStackPtr) xmlMalloc(sizeof(xmlC14NVisibleNsStack));
    if (ret == NULL)
	return(NULL);
    memset(ret, 0, sizeof(xmlC14NVisibleNsStack));
    return(ret);
}

static void
xmlC14NVisibleNsStackDestroy(xmlC14NVisibleNsStackPtr cur) {
    if(cur == NULL) {
        xmlC14NErrParam(NULL);
        return;
    }
    if(cur->nsTab != NULL) {
	memset(cur->nsTab, 0, cur->nsMax * sizeof(xmlNsPtr));
	xmlFree(cur->nsTab);
    }
    if(cur->nodeTab != NULL) {
	memset(cur->nodeTab, 0, cur->nsMax * sizeof(xmlNodePtr));
	xmlFree(cur->nodeTab);
    }
    memset(cur, 0, sizeof(xmlC14NVisibleNsStack));
    xmlFree(cur);

}

static int
xmlC14NVisibleNsStackAdd(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns, xmlNodePtr node) {
    if((cur == NULL) ||
       ((cur->nsTab == NULL) && (cur->nodeTab != NULL)) ||
       ((cur->nsTab != NULL) && (cur->nodeTab == NULL)))
	return (1);

    if (cur->nsMax <= cur->nsCurEnd) {
	xmlNsPtr *tmp1;
        xmlNodePtr *tmp2;
	int newSize;

        newSize = xmlGrowCapacity(cur->nsMax,
                                  sizeof(tmp1[0]) + sizeof(tmp2[0]),
                                  XML_NAMESPACES_DEFAULT, XML_MAX_ITEMS);

	tmp1 = xmlRealloc(cur->nsTab, newSize * sizeof(tmp1[0]));
	if (tmp1 == NULL)
	    return (-1);
	cur->nsTab = tmp1;

	tmp2 = xmlRealloc(cur->nodeTab, newSize * sizeof(tmp2[0]));
	if (tmp2 == NULL)
	    return (-1);
	cur->nodeTab = tmp2;

	cur->nsMax = newSize;
    }
    cur->nsTab[cur->nsCurEnd] = ns;
    cur->nodeTab[cur->nsCurEnd] = node;

    ++cur->nsCurEnd;

    return (0);
}

static void
xmlC14NVisibleNsStackSave(xmlC14NVisibleNsStackPtr cur, xmlC14NVisibleNsStackPtr state) {
    if((cur == NULL) || (state == NULL)) {
        xmlC14NErrParam(NULL);
	return;
    }

    state->nsCurEnd = cur->nsCurEnd;
    state->nsPrevStart = cur->nsPrevStart;
    state->nsPrevEnd = cur->nsPrevEnd;
}

static void
xmlC14NVisibleNsStackRestore(xmlC14NVisibleNsStackPtr cur, xmlC14NVisibleNsStackPtr state) {
    if((cur == NULL) || (state == NULL)) {
        xmlC14NErrParam(NULL);
	return;
    }
    cur->nsCurEnd = state->nsCurEnd;
    cur->nsPrevStart = state->nsPrevStart;
    cur->nsPrevEnd = state->nsPrevEnd;
}

static void
xmlC14NVisibleNsStackShift(xmlC14NVisibleNsStackPtr cur) {
    if(cur == NULL) {
        xmlC14NErrParam(NULL);
	return;
    }
    cur->nsPrevStart = cur->nsPrevEnd;
    cur->nsPrevEnd = cur->nsCurEnd;
}

static int
xmlC14NStrEqual(const xmlChar *str1, const xmlChar *str2) {
    if (str1 == str2) return(1);
    if (str1 == NULL) return((*str2) == '\0');
    if (str2 == NULL) return((*str1) == '\0');
    do {
	if (*str1++ != *str2) return(0);
    } while (*str2++);
    return(1);
}

/**
 * Checks whether the given namespace was already rendered or not
 *
 * @param cur  		the visible stack
 * @param ns  			the namespace to check
 * @returns 1 if we already wrote this namespace or 0 otherwise
 */
static int
xmlC14NVisibleNsStackFind(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns)
{
    int i;
    const xmlChar *prefix;
    const xmlChar *href;
    int has_empty_ns;

    if(cur == NULL) {
        xmlC14NErrParam(NULL);
        return (0);
    }

    /*
     * if the default namespace xmlns="" is not defined yet then
     * we do not want to print it out
     */
    prefix = ((ns == NULL) || (ns->prefix == NULL)) ? BAD_CAST "" : ns->prefix;
    href = ((ns == NULL) || (ns->href == NULL)) ? BAD_CAST "" : ns->href;
    has_empty_ns = (xmlC14NStrEqual(prefix, NULL) && xmlC14NStrEqual(href, NULL));

    if (cur->nsTab != NULL) {
	int start = (has_empty_ns) ? 0 : cur->nsPrevStart;
        for (i = cur->nsCurEnd - 1; i >= start; --i) {
            xmlNsPtr ns1 = cur->nsTab[i];

	    if(xmlC14NStrEqual(prefix, (ns1 != NULL) ? ns1->prefix : NULL)) {
		return(xmlC14NStrEqual(href, (ns1 != NULL) ? ns1->href : NULL));
	    }
        }
    }
    return(has_empty_ns);
}

static int
xmlExcC14NVisibleNsStackFind(xmlC14NVisibleNsStackPtr cur, xmlNsPtr ns, xmlC14NCtxPtr ctx) {
    int i;
    const xmlChar *prefix;
    const xmlChar *href;
    int has_empty_ns;

    if(cur == NULL) {
        xmlC14NErrParam(ctx);
        return (0);
    }

    /*
     * if the default namespace xmlns="" is not defined yet then
     * we do not want to print it out
     */
    prefix = ((ns == NULL) || (ns->prefix == NULL)) ? BAD_CAST "" : ns->prefix;
    href = ((ns == NULL) || (ns->href == NULL)) ? BAD_CAST "" : ns->href;
    has_empty_ns = (xmlC14NStrEqual(prefix, NULL) && xmlC14NStrEqual(href, NULL));

    if (cur->nsTab != NULL) {
	int start = 0;
        for (i = cur->nsCurEnd - 1; i >= start; --i) {
            xmlNsPtr ns1 = cur->nsTab[i];

	    if(xmlC14NStrEqual(prefix, (ns1 != NULL) ? ns1->prefix : NULL)) {
		if(xmlC14NStrEqual(href, (ns1 != NULL) ? ns1->href : NULL)) {
		    return(xmlC14NIsVisible(ctx, ns1, cur->nodeTab[i]));
		} else {
		    return(0);
		}
	    }
        }
    }
    return(has_empty_ns);
}




/**
 * Checks whether the given namespace is a default "xml:" namespace
 * with href="http://www.w3.org/XML/1998/namespace"
 *
 * @param ns  		the namespace to check
 * @returns 1 if the node is default or 0 otherwise
 */

/* todo: make it a define? */
static int
xmlC14NIsXmlNs(xmlNsPtr ns)
{
    return ((ns != NULL) &&
            (xmlStrEqual(ns->prefix, BAD_CAST "xml")) &&
            (xmlStrEqual(ns->href, XML_XML_NAMESPACE)));
}


/**
 * Compares the namespaces by names (prefixes).
 *
 * @param data1  		the pointer to first namespace
 * @param data2  		the pointer to second namespace
 * @returns -1 if ns1 < ns2, 0 if ns1 == ns2 or 1 if ns1 > ns2.
 */
static int
xmlC14NNsCompare(const void *data1, const void *data2)
{
    const xmlNs *ns1 = data1;
    const xmlNs *ns2 = data2;
    if (ns1 == ns2)
        return (0);
    if (ns1 == NULL)
        return (-1);
    if (ns2 == NULL)
        return (1);

    return (xmlStrcmp(ns1->prefix, ns2->prefix));
}


/**
 * Prints the given namespace to the output buffer from C14N context.
 *
 * @param ns  			the pointer to namespace
 * @param ctx  		the C14N context
 * @returns 1 on success or 0 on fail.
 */
static int
xmlC14NPrintNamespaces(const xmlNs *ns, xmlC14NCtxPtr ctx)
{

    if ((ns == NULL) || (ctx == NULL)) {
        xmlC14NErrParam(ctx);
        return 0;
    }

    if (ns->prefix != NULL) {
        xmlOutputBufferWriteString(ctx->buf, " xmlns:");
        xmlOutputBufferWriteString(ctx->buf, (const char *) ns->prefix);
        xmlOutputBufferWriteString(ctx->buf, "=");
    } else {
        xmlOutputBufferWriteString(ctx->buf, " xmlns=");
    }
    if(ns->href != NULL) {
	xmlOutputBufferWriteQuotedString(ctx->buf, ns->href);
    } else {
    	xmlOutputBufferWriteString(ctx->buf, "\"\"");
    }
    return (1);
}

static int
xmlC14NPrintNamespacesWalker(const void *ns, void *ctx) {
    return xmlC14NPrintNamespaces(ns, ctx);
}

/**
 * Prints out canonical namespace axis of the current node to the
 * buffer from C14N context as follows
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Namespace Axis
 * Consider a list L containing only namespace nodes in the
 * axis and in the node-set in lexicographic order (ascending). To begin
 * processing L, if the first node is not the default namespace node (a node
 * with no namespace URI and no local name), then generate a space followed
 * by xmlns="" if and only if the following conditions are met:
 *    - the element E that owns the axis is in the node-set
 *    - The nearest ancestor element of E in the node-set has a default
 *	    namespace node in the node-set (default namespace nodes always
 *      have non-empty values in XPath)
 * The latter condition eliminates unnecessary occurrences of xmlns="" in
 * the canonical form since an element only receives an xmlns="" if its
 * default namespace is empty and if it has an immediate parent in the
 * canonical form that has a non-empty default namespace. To finish
 * processing  L, simply process every namespace node in L, except omit
 * namespace node with local name xml, which defines the xml prefix,
 * if its string value is http://www.w3.org/XML/1998/namespace.
 *
 * Exclusive XML Canonicalization v 1.0 (http://www.w3.org/TR/xml-exc-c14n)
 * Canonical XML applied to a document subset requires the search of the
 * ancestor nodes of each orphan element node for attributes in the xml
 * namespace, such as xml:lang and xml:space. These are copied into the
 * element node except if a declaration of the same attribute is already
 * in the attribute axis of the element (whether or not it is included in
 * the document subset). This search and copying are omitted from the
 * Exclusive XML Canonicalization method.
 *
 * @param ctx  		the C14N context
 * @param cur  		the current node
 * @param visible  		the visibility of node
 * @returns 0 on success or -1 on fail.
 */
static int
xmlC14NProcessNamespacesAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur, int visible)
{
    xmlNodePtr n;
    xmlNsPtr ns, tmp;
    xmlListPtr list;
    int already_rendered;
    int has_empty_ns = 0;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    /*
     * Create a sorted list to store element namespaces
     */
    list = xmlListCreate(NULL, xmlC14NNsCompare);
    if (list == NULL) {
        xmlC14NErrMemory(ctx);
        return (-1);
    }

    /* check all namespaces */
    for(n = cur; n != NULL; n = n->parent) {
	for(ns = n->nsDef; ns != NULL; ns = ns->next) {
	    tmp = xmlSearchNs(cur->doc, cur, ns->prefix);

	    if((tmp == ns) && !xmlC14NIsXmlNs(ns) && xmlC14NIsVisible(ctx, ns, cur)) {
		already_rendered = xmlC14NVisibleNsStackFind(ctx->ns_rendered, ns);
		if(visible) {
	            if (xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur) < 0) {
                        xmlC14NErrMemory(ctx);
                        goto error;
                    }
		}
		if(!already_rendered) {
		    xmlListInsert(list, ns);
		}
		if(xmlStrlen(ns->prefix) == 0) {
		    has_empty_ns = 1;
		}
	    }
	}
    }

    /**
     * if the first node is not the default namespace node (a node with no
     * namespace URI and no local name), then generate a space followed by
     * xmlns="" if and only if the following conditions are met:
     *  - the element E that owns the axis is in the node-set
     *  - the nearest ancestor element of E in the node-set has a default
     *     namespace node in the node-set (default namespace nodes always
     *     have non-empty values in XPath)
     */
    if(visible && !has_empty_ns) {
        xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));
        if(!xmlC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default)) {
	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    }


    /*
     * print out all elements from list
     */
    xmlListWalk(list, xmlC14NPrintNamespacesWalker, (void *) ctx);

    /*
     * Cleanup
     */
error:
    xmlListDelete(list);
    return (0);
}


/**
 * Prints out exclusive canonical namespace axis of the current node to the
 * buffer from C14N context as follows
 *
 * Exclusive XML Canonicalization
 * http://www.w3.org/TR/xml-exc-c14n
 *
 * If the element node is in the XPath subset then output the node in
 * accordance with Canonical XML except for namespace nodes which are
 * rendered as follows:
 *
 * 1. Render each namespace node iff:
 *    * it is visibly utilized by the immediate parent element or one of
 *      its attributes, or is present in InclusiveNamespaces PrefixList, and
 *    * its prefix and value do not appear in ns_rendered. ns_rendered is
 *      obtained by popping the state stack in order to obtain a list of
 *      prefixes and their values which have already been rendered by
 *      an output ancestor of the namespace node's parent element.
 * 2. Append the rendered namespace node to the list ns_rendered of namespace
 * nodes rendered by output ancestors. Push ns_rendered on state stack and
 * recurse.
 * 3. After the recursion returns, pop thestate stack.
 *
 *
 * @param ctx  		the C14N context
 * @param cur  		the current node
 * @param visible  		the visibility of node
 * @returns 0 on success or -1 on fail.
 */
static int
xmlExcC14NProcessNamespacesAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur, int visible)
{
    xmlNsPtr ns;
    xmlListPtr list;
    xmlAttrPtr attr;
    int already_rendered;
    int has_empty_ns = 0;
    int has_visibly_utilized_empty_ns = 0;
    int has_empty_ns_in_inclusive_list = 0;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    if(!xmlC14NIsExclusive(ctx)) {
        xmlC14NErrParam(ctx);
        return (-1);

    }

    /*
     * Create a sorted list to store element namespaces
     */
    list = xmlListCreate(NULL, xmlC14NNsCompare);
    if (list == NULL) {
        xmlC14NErrMemory(ctx);
        return (-1);
    }

    /*
     * process inclusive namespaces:
     * All namespace nodes appearing on inclusive ns list are
     * handled as provided in Canonical XML
     */
    if(ctx->inclusive_ns_prefixes != NULL) {
	xmlChar *prefix;
	int i;

	for (i = 0; ctx->inclusive_ns_prefixes[i] != NULL; ++i) {
	    prefix = ctx->inclusive_ns_prefixes[i];
	    /*
	     * Special values for namespace with empty prefix
	     */
            if (xmlStrEqual(prefix, BAD_CAST "#default")
                || xmlStrEqual(prefix, BAD_CAST "")) {
                prefix = NULL;
		has_empty_ns_in_inclusive_list = 1;
            }

	    ns = xmlSearchNs(cur->doc, cur, prefix);
	    if((ns != NULL) && !xmlC14NIsXmlNs(ns) && xmlC14NIsVisible(ctx, ns, cur)) {
		already_rendered = xmlC14NVisibleNsStackFind(ctx->ns_rendered, ns);
		if(visible) {
		    if (xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur) < 0) {
                        xmlC14NErrMemory(ctx);
                        goto error;
                    }
		}
		if(!already_rendered) {
		    xmlListInsert(list, ns);
		}
		if(xmlStrlen(ns->prefix) == 0) {
		    has_empty_ns = 1;
		}
	    }
	}
    }

    /* add node namespace */
    if(cur->ns != NULL) {
	ns = cur->ns;
    } else {
        ns = xmlSearchNs(cur->doc, cur, NULL);
	has_visibly_utilized_empty_ns = 1;
    }
    if((ns != NULL) && !xmlC14NIsXmlNs(ns)) {
	if(visible && xmlC14NIsVisible(ctx, ns, cur)) {
	    if(!xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, ns, ctx)) {
		xmlListInsert(list, ns);
	    }
	}
	if(visible) {
	    if (xmlC14NVisibleNsStackAdd(ctx->ns_rendered, ns, cur) < 0) {
                xmlC14NErrMemory(ctx);
                goto error;
            }
	}
	if(xmlStrlen(ns->prefix) == 0) {
	    has_empty_ns = 1;
	}
    }


    /* add attributes */
    for(attr = cur->properties; attr != NULL; attr = attr->next) {
        /*
         * we need to check that attribute is visible and has non
         * default namespace (XML Namespaces: "default namespaces
	 * do not apply directly to attributes")
         */
	if((attr->ns != NULL) && !xmlC14NIsXmlNs(attr->ns) && xmlC14NIsVisible(ctx, attr, cur)) {
	    already_rendered = xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, attr->ns, ctx);
	    if (xmlC14NVisibleNsStackAdd(ctx->ns_rendered, attr->ns, cur) < 0) {
                xmlC14NErrMemory(ctx);
                goto error;
            }
	    if(!already_rendered && visible) {
		xmlListInsert(list, attr->ns);
	    }
	    if(xmlStrlen(attr->ns->prefix) == 0) {
		has_empty_ns = 1;
	    }
	} else if((attr->ns != NULL) && (xmlStrlen(attr->ns->prefix) == 0) && (xmlStrlen(attr->ns->href) == 0)) {
	    has_visibly_utilized_empty_ns = 1;
	}
    }

    /*
     * Process xmlns=""
     */
    if(visible && has_visibly_utilized_empty_ns &&
	    !has_empty_ns && !has_empty_ns_in_inclusive_list) {
        xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));

        already_rendered = xmlExcC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default, ctx);
	if(!already_rendered) {
	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    } else if(visible && !has_empty_ns && has_empty_ns_in_inclusive_list) {
        xmlNs ns_default;

        memset(&ns_default, 0, sizeof(ns_default));
        if(!xmlC14NVisibleNsStackFind(ctx->ns_rendered, &ns_default)) {
	    xmlC14NPrintNamespaces(&ns_default, ctx);
	}
    }



    /*
     * print out all elements from list
     */
    xmlListWalk(list, xmlC14NPrintNamespacesWalker, (void *) ctx);

    /*
     * Cleanup
     */
error:
    xmlListDelete(list);
    return (0);
}


/**
 * Checks whether the given attribute is a default "xml:" namespace
 * with href="http://www.w3.org/XML/1998/namespace"
 *
 * @param attr  		the attr to check
 * @returns 1 if the node is default or 0 otherwise
 */

/* todo: make it a define? */
static int
xmlC14NIsXmlAttr(xmlAttrPtr attr)
{
    return ((attr->ns != NULL) &&
           (xmlC14NIsXmlNs(attr->ns) != 0));
}


/**
 * Prints the given attribute to the output buffer from C14N context.
 *
 * @param data1  		the pointer tls o first attr
 * @param data2  		the pointer to second attr
 * @returns -1 if attr1 < attr2, 0 if attr1 == attr2 or 1 if attr1 > attr2.
 */
static int
xmlC14NAttrsCompare(const void *data1, const void *data2)
{
    const xmlAttr *attr1 = data1;
    const xmlAttr *attr2 = data2;
    int ret = 0;

    /*
     * Simple cases
     */
    if (attr1 == attr2)
        return (0);
    if (attr1 == NULL)
        return (-1);
    if (attr2 == NULL)
        return (1);
    if (attr1->ns == attr2->ns) {
        return (xmlStrcmp(attr1->name, attr2->name));
    }

    /*
     * Attributes in the default namespace are first
     * because the default namespace is not applied to
     * unqualified attributes
     */
    if (attr1->ns == NULL)
        return (-1);
    if (attr2->ns == NULL)
        return (1);
    if (attr1->ns->prefix == NULL)
        return (-1);
    if (attr2->ns->prefix == NULL)
        return (1);

    ret = xmlStrcmp(attr1->ns->href, attr2->ns->href);
    if (ret == 0) {
        ret = xmlStrcmp(attr1->name, attr2->name);
    }
    return (ret);
}


/**
 * Prints out canonical attribute urrent node to the
 * buffer from C14N context as follows
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * @param data  		the pointer to attr
 * @param user  		the C14N context
 * @returns 1 on success or 0 on fail.
 */
static int
xmlC14NPrintAttrs(const void *data, void *user)
{
    const xmlAttr *attr = data;
    xmlC14NCtxPtr ctx = (xmlC14NCtxPtr) user;
    xmlChar *value;
    xmlChar *buffer;

    if ((attr == NULL) || (ctx == NULL)) {
        xmlC14NErrParam(ctx);
        return (0);
    }

    xmlOutputBufferWriteString(ctx->buf, " ");
    if (attr->ns != NULL && xmlStrlen(attr->ns->prefix) > 0) {
        xmlOutputBufferWriteString(ctx->buf,
                                   (const char *) attr->ns->prefix);
        xmlOutputBufferWriteString(ctx->buf, ":");
    }
    xmlOutputBufferWriteString(ctx->buf, (const char *) attr->name);
    xmlOutputBufferWriteString(ctx->buf, "=\"");

    value = xmlNodeListGetString(ctx->doc, attr->children, 1);
    /* todo: should we log an error if value==NULL ? */
    if (value != NULL) {
        buffer = xmlC11NNormalizeAttr(value);
        xmlFree(value);
        if (buffer != NULL) {
            xmlOutputBufferWriteString(ctx->buf, (const char *) buffer);
            xmlFree(buffer);
        } else {
            xmlC14NErrMemory(ctx);
            return (0);
        }
    }
    xmlOutputBufferWriteString(ctx->buf, "\"");
    return (1);
}

/**
 * Finds an attribute in a hidden parent node.
 *
 * @returns a pointer to the attribute node (if found) or NULL otherwise.
 */
static xmlAttrPtr
xmlC14NFindHiddenParentAttr(xmlC14NCtxPtr ctx, xmlNodePtr cur, const xmlChar * name, const xmlChar * ns)
{
    xmlAttrPtr res;
    while((cur != NULL) && (!xmlC14NIsVisible(ctx, cur, cur->parent))) {
        res = xmlHasNsProp(cur, name, ns);
        if(res != NULL) {
            return res;
        }

        cur = cur->parent;
    }

    return NULL;
}

/**
 * Fixes up the xml:base attribute
 *
 * @returns the newly created attribute or NULL
 */
static xmlAttrPtr
xmlC14NFixupBaseAttr(xmlC14NCtxPtr ctx, xmlAttrPtr xml_base_attr)
{
    xmlChar * res = NULL;
    xmlNodePtr cur;
    xmlAttrPtr attr;
    xmlChar * tmp_str;
    xmlChar * tmp_str2;
    int tmp_str_len;

    if ((ctx == NULL) || (xml_base_attr == NULL) || (xml_base_attr->parent == NULL)) {
        xmlC14NErrParam(ctx);
        return (NULL);
    }

    /* start from current value */
    res = xmlNodeListGetString(ctx->doc, xml_base_attr->children, 1);
    if(res == NULL) {
        xmlC14NErrMemory(ctx);
        return (NULL);
    }

    /* go up the stack until we find a node that we rendered already */
    cur = xml_base_attr->parent->parent;
    while((cur != NULL) && (!xmlC14NIsVisible(ctx, cur, cur->parent))) {
        int code;

        attr = xmlHasNsProp(cur, BAD_CAST "base", XML_XML_NAMESPACE);
        if(attr != NULL) {
            /* get attr value */
            tmp_str = xmlNodeListGetString(ctx->doc, attr->children, 1);
            if(tmp_str == NULL) {
                xmlFree(res);

                xmlC14NErrMemory(ctx);
                return (NULL);
            }

            /* we need to add '/' if our current base uri ends with '..' or '.'
            to ensure that we are forced to go "up" all the time */
            tmp_str_len = xmlStrlen(tmp_str);
            if(tmp_str_len > 1 && tmp_str[tmp_str_len - 2] == '.') {
                tmp_str2 = xmlStrcat(tmp_str, BAD_CAST "/");
                if(tmp_str2 == NULL) {
                    xmlFree(tmp_str);
                    xmlFree(res);

                    xmlC14NErrMemory(ctx);
                    return (NULL);
                }

                tmp_str = tmp_str2;
            }

            /* build uri */
            code = xmlBuildURISafe(res, tmp_str, &tmp_str2);
            if (code != 0) {
                xmlFree(tmp_str);
                xmlFree(res);

                if (code < 0)
                    xmlC14NErrMemory(ctx);
                else
                    xmlC14NErr(ctx, cur, XML_ERR_INVALID_URI,
                               "processing xml:base attribute - "
                               "can't construct uri");
                return (NULL);
            }

            /* cleanup and set the new res */
            xmlFree(tmp_str);
            xmlFree(res);
            res = tmp_str2;
        }

        /* next */
        cur = cur->parent;
    }

    /* check if result uri is empty or not */
    if((res == NULL) || xmlStrEqual(res, BAD_CAST "")) {
        xmlFree(res);
        return (NULL);
    }

    /* create and return the new attribute node */
    attr = xmlNewNsProp(NULL, xml_base_attr->ns, BAD_CAST "base", res);
    if(attr == NULL) {
        xmlFree(res);

        xmlC14NErrMemory(ctx);
        return (NULL);
    }

    /* done */
    xmlFree(res);
    return (attr);
}

/**
 * Prints out canonical attribute axis of the current node to the
 * buffer from C14N context as follows
 *
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Attribute Axis
 * In lexicographic order (ascending), process each node that
 * is in the element's attribute axis and in the node-set.
 *
 * The processing of an element node E MUST be modified slightly
 * when an XPath node-set is given as input and the element's
 * parent is omitted from the node-set.
 *
 *
 * Exclusive XML Canonicalization v 1.0 (http://www.w3.org/TR/xml-exc-c14n)
 *
 * Canonical XML applied to a document subset requires the search of the
 * ancestor nodes of each orphan element node for attributes in the xml
 * namespace, such as xml:lang and xml:space. These are copied into the
 * element node except if a declaration of the same attribute is already
 * in the attribute axis of the element (whether or not it is included in
 * the document subset). This search and copying are omitted from the
 * Exclusive XML Canonicalization method.
 *
 * @param ctx  		the C14N context
 * @param cur  		the current node
 * @param parent_visible  	the visibility of the parent node
 * @returns 0 on success or -1 on fail.
 */
static int
xmlC14NProcessAttrsAxis(xmlC14NCtxPtr ctx, xmlNodePtr cur, int parent_visible)
{
    xmlAttrPtr attr;
    xmlListPtr list;
    xmlAttrPtr attrs_to_delete = NULL;

    /* special processing for 1.1 spec */
    xmlAttrPtr xml_base_attr = NULL;
    xmlAttrPtr xml_lang_attr = NULL;
    xmlAttrPtr xml_space_attr = NULL;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    /*
     * Create a sorted list to store element attributes
     */
    list = xmlListCreate(NULL, xmlC14NAttrsCompare);
    if (list == NULL) {
        xmlC14NErrMemory(ctx);
        return (-1);
    }

    switch(ctx->mode) {
    case XML_C14N_1_0:
        /* The processing of an element node E MUST be modified slightly when an XPath node-set is
         * given as input and the element's parent is omitted from the node-set. The method for processing
         * the attribute axis of an element E in the node-set is enhanced. All element nodes along E's
         * ancestor axis are examined for nearest occurrences of attributes in the xml namespace, such
         * as xml:lang and xml:space (whether or not they are in the node-set). From this list of attributes,
         * remove any that are in E's attribute axis (whether or not they are in the node-set). Then,
         * lexicographically merge this attribute list with the nodes of E's attribute axis that are in
         * the node-set. The result of visiting the attribute axis is computed by processing the attribute
         * nodes in this merged attribute list.
         */

        /*
         * Add all visible attributes from current node.
         */
        attr = cur->properties;
        while (attr != NULL) {
            /* check that attribute is visible */
            if (xmlC14NIsVisible(ctx, attr, cur)) {
                xmlListInsert(list, attr);
            }
            attr = attr->next;
        }

        /*
         * Handle xml attributes
         */
        if (parent_visible && (cur->parent != NULL) &&
            (!xmlC14NIsVisible(ctx, cur->parent, cur->parent->parent)))
        {
            xmlNodePtr tmp;

            /*
             * If XPath node-set is not specified then the parent is always
             * visible!
             */
            tmp = cur->parent;
            while (tmp != NULL) {
                attr = tmp->properties;
                while (attr != NULL) {
                    if (xmlC14NIsXmlAttr(attr) != 0) {
                        if (xmlListSearch(list, attr) == NULL) {
                            xmlListInsert(list, attr);
                        }
                    }
                    attr = attr->next;
                }
                tmp = tmp->parent;
            }
        }

        /* done */
        break;
    case XML_C14N_EXCLUSIVE_1_0:
        /* attributes in the XML namespace, such as xml:lang and xml:space
         * are not imported into orphan nodes of the document subset
         */

        /*
         * Add all visible attributes from current node.
         */
        attr = cur->properties;
        while (attr != NULL) {
            /* check that attribute is visible */
            if (xmlC14NIsVisible(ctx, attr, cur)) {
                xmlListInsert(list, attr);
            }
            attr = attr->next;
        }

        /* do nothing special for xml attributes */
        break;
    case XML_C14N_1_1:
        /* The processing of an element node E MUST be modified slightly when an XPath node-set is
         * given as input and some of the element's ancestors are omitted from the node-set.
         *
         * Simple inheritable attributes are attributes that have a value that requires at most a simple
         * redeclaration. This redeclaration is done by supplying a new value in the child axis. The
         * redeclaration of a simple inheritable attribute A contained in one of E's ancestors is done
         * by supplying a value to an attribute Ae inside E with the same name. Simple inheritable attributes
         * are xml:lang and xml:space.
         *
         * The method for processing the attribute axis of an element E in the node-set is hence enhanced.
         * All element nodes along E's ancestor axis are examined for the nearest occurrences of simple
         * inheritable attributes in the xml namespace, such as xml:lang and xml:space (whether or not they
         * are in the node-set). From this list of attributes, any simple inheritable attributes that are
         * already in E's attribute axis (whether or not they are in the node-set) are removed. Then,
         * lexicographically merge this attribute list with the nodes of E's attribute axis that are in
         * the node-set. The result of visiting the attribute axis is computed by processing the attribute
         * nodes in this merged attribute list.
         *
         * The xml:id attribute is not a simple inheritable attribute and no processing of these attributes is
         * performed.
         *
         * The xml:base attribute is not a simple inheritable attribute and requires special processing beyond
         * a simple redeclaration.
         *
         * Attributes in the XML namespace other than xml:base, xml:id, xml:lang, and xml:space MUST be processed
         * as ordinary attributes.
         */

        /*
         * Add all visible attributes from current node.
         */
        attr = cur->properties;
        while (attr != NULL) {
            /* special processing for XML attribute kiks in only when we have invisible parents */
            if ((!parent_visible) || (xmlC14NIsXmlAttr(attr) == 0)) {
                /* check that attribute is visible */
                if (xmlC14NIsVisible(ctx, attr, cur)) {
                    xmlListInsert(list, attr);
                }
            } else {
                int matched = 0;

                /* check for simple inheritance attributes */
                if((!matched) && (xml_lang_attr == NULL) && xmlStrEqual(attr->name, BAD_CAST "lang")) {
                    xml_lang_attr = attr;
                    matched = 1;
                }
                if((!matched) && (xml_space_attr == NULL) && xmlStrEqual(attr->name, BAD_CAST "space")) {
                    xml_space_attr = attr;
                    matched = 1;
                }

                /* check for base attr */
                if((!matched) && (xml_base_attr == NULL) && xmlStrEqual(attr->name, BAD_CAST "base")) {
                    xml_base_attr = attr;
                    matched = 1;
                }

                /* otherwise, it is a normal attribute, so just check if it is visible */
                if((!matched) && xmlC14NIsVisible(ctx, attr, cur)) {
                    xmlListInsert(list, attr);
                }
            }

            /* move to the next one */
            attr = attr->next;
        }

        /* special processing for XML attribute kiks in only when we have invisible parents */
        if ((parent_visible)) {

            /* simple inheritance attributes - copy */
            if(xml_lang_attr == NULL) {
                xml_lang_attr = xmlC14NFindHiddenParentAttr(ctx, cur->parent, BAD_CAST "lang", XML_XML_NAMESPACE);
            }
            if(xml_lang_attr != NULL) {
                xmlListInsert(list, xml_lang_attr);
            }
            if(xml_space_attr == NULL) {
                xml_space_attr = xmlC14NFindHiddenParentAttr(ctx, cur->parent, BAD_CAST "space", XML_XML_NAMESPACE);
            }
            if(xml_space_attr != NULL) {
                xmlListInsert(list, xml_space_attr);
            }

            /* base uri attribute - fix up */
            if(xml_base_attr == NULL) {
                /* if we don't have base uri attribute, check if we have a "hidden" one above */
                xml_base_attr = xmlC14NFindHiddenParentAttr(ctx, cur->parent, BAD_CAST "base", XML_XML_NAMESPACE);
            }
            if(xml_base_attr != NULL) {
                xml_base_attr = xmlC14NFixupBaseAttr(ctx, xml_base_attr);
                if(xml_base_attr != NULL) {
                    xmlListInsert(list, xml_base_attr);

                    /* note that we MUST delete returned attr node ourselves! */
                    xml_base_attr->next = attrs_to_delete;
                    attrs_to_delete = xml_base_attr;
                }
            }
        }

        /* done */
        break;
    }

    /*
     * print out all elements from list
     */
    xmlListWalk(list, xmlC14NPrintAttrs, (void *) ctx);

    /*
     * Cleanup
     */
    xmlFreePropList(attrs_to_delete);
    xmlListDelete(list);
    return (0);
}

/**
 * Checks that current element node has no relative namespaces defined
 *
 * @param ctx  		the C14N context
 * @param cur  		the current element node
 * @returns 0 if the node has no relative namespaces or -1 otherwise.
 */
static int
xmlC14NCheckForRelativeNamespaces(xmlC14NCtxPtr ctx, xmlNodePtr cur)
{
    xmlNsPtr ns;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    ns = cur->nsDef;
    while (ns != NULL) {
        if (xmlStrlen(ns->href) > 0) {
            xmlURIPtr uri;
            int code;

            code = xmlParseURISafe((const char *) ns->href, &uri);
            if (uri == NULL) {
                if (code < 0)
                    xmlC14NErrMemory(ctx);
                else
                    xmlC14NErr(ctx, cur, XML_ERR_INVALID_URI,
                               "parsing namespace uri");
                return (-1);
            }
            if (xmlStrlen((const xmlChar *) uri->scheme) == 0) {
                xmlC14NErrRelativeNamespace(ctx, uri->scheme);
                xmlFreeURI(uri);
                return (-1);
            }
            xmlFreeURI(uri);
        }
        ns = ns->next;
    }
    return (0);
}

/**
 * Canonical XML v 1.0 (http://www.w3.org/TR/xml-c14n)
 *
 * Element Nodes
 * If the element is not in the node-set, then the result is obtained
 * by processing the namespace axis, then the attribute axis, then
 * processing the child nodes of the element that are in the node-set
 * (in document order). If the element is in the node-set, then the result
 * is an open angle bracket (<), the element QName, the result of
 * processing the namespace axis, the result of processing the attribute
 * axis, a close angle bracket (>), the result of processing the child
 * nodes of the element that are in the node-set (in document order), an
 * open angle bracket, a forward slash (/), the element QName, and a close
 * angle bracket.
 *
 * @param ctx  		the pointer to C14N context object
 * @param cur  		the node to process
 * @param visible  		this node is visible
 * @returns non-negative value on success or negative value on fail
 */
static int
xmlC14NProcessElementNode(xmlC14NCtxPtr ctx, xmlNodePtr cur, int visible)
{
    int ret;
    xmlC14NVisibleNsStack state;
    int parent_is_doc = 0;

    if ((ctx == NULL) || (cur == NULL) || (cur->type != XML_ELEMENT_NODE)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    /*
     * Check relative relative namespaces:
     * implementations of XML canonicalization MUST report an operation
     * failure on documents containing relative namespace URIs.
     */
    if (xmlC14NCheckForRelativeNamespaces(ctx, cur) < 0)
        return (-1);

    /*
     * Save ns_rendered stack position
     */
    memset(&state, 0, sizeof(state));
    xmlC14NVisibleNsStackSave(ctx->ns_rendered, &state);

    if (visible) {
        if (ctx->parent_is_doc) {
	    /* save this flag into the stack */
	    parent_is_doc = ctx->parent_is_doc;
	    ctx->parent_is_doc = 0;
            ctx->pos = XMLC14N_INSIDE_DOCUMENT_ELEMENT;
        }
        xmlOutputBufferWriteString(ctx->buf, "<");

        if ((cur->ns != NULL) && (xmlStrlen(cur->ns->prefix) > 0)) {
            xmlOutputBufferWriteString(ctx->buf,
                                       (const char *) cur->ns->prefix);
            xmlOutputBufferWriteString(ctx->buf, ":");
        }
        xmlOutputBufferWriteString(ctx->buf, (const char *) cur->name);
    }

    if (!xmlC14NIsExclusive(ctx)) {
        ret = xmlC14NProcessNamespacesAxis(ctx, cur, visible);
    } else {
        ret = xmlExcC14NProcessNamespacesAxis(ctx, cur, visible);
    }
    if (ret < 0)
        return (-1);
    /* todo: shouldn't this go to "visible only"? */
    if(visible) {
	xmlC14NVisibleNsStackShift(ctx->ns_rendered);
    }

    ret = xmlC14NProcessAttrsAxis(ctx, cur, visible);
    if (ret < 0)
	return (-1);

    if (visible) {
        xmlOutputBufferWriteString(ctx->buf, ">");
    }
    if (cur->children != NULL) {
        ret = xmlC14NProcessNodeList(ctx, cur->children);
        if (ret < 0)
            return (-1);
    }
    if (visible) {
        xmlOutputBufferWriteString(ctx->buf, "</");
        if ((cur->ns != NULL) && (xmlStrlen(cur->ns->prefix) > 0)) {
            xmlOutputBufferWriteString(ctx->buf,
                                       (const char *) cur->ns->prefix);
            xmlOutputBufferWriteString(ctx->buf, ":");
        }
        xmlOutputBufferWriteString(ctx->buf, (const char *) cur->name);
        xmlOutputBufferWriteString(ctx->buf, ">");
        if (parent_is_doc) {
	    /* restore this flag from the stack for next node */
            ctx->parent_is_doc = parent_is_doc;
	    ctx->pos = XMLC14N_AFTER_DOCUMENT_ELEMENT;
        }
    }

    /*
     * Restore ns_rendered stack position
     */
    xmlC14NVisibleNsStackRestore(ctx->ns_rendered, &state);
    return (0);
}

/**
 * Processes the given node
 *
 * @param ctx  		the pointer to C14N context object
 * @param cur  		the node to process
 * @returns non-negative value on success or negative value on fail
 */
static int
xmlC14NProcessNode(xmlC14NCtxPtr ctx, xmlNodePtr cur)
{
    int ret = 0;
    int visible;

    if ((ctx == NULL) || (cur == NULL)) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    visible = xmlC14NIsVisible(ctx, cur, cur->parent);
    switch (cur->type) {
        case XML_ELEMENT_NODE:
            ret = xmlC14NProcessElementNode(ctx, cur, visible);
            break;
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
            /*
             * Text Nodes
             * the string value, except all ampersands are replaced
             * by &amp;, all open angle brackets (<) are replaced by &lt;, all closing
             * angle brackets (>) are replaced by &gt;, and all #xD characters are
             * replaced by &#xD;.
             */
            /* cdata sections are processed as text nodes */
            /* todo: verify that cdata sections are included in XPath nodes set */
            if ((visible) && (cur->content != NULL)) {
                xmlChar *buffer;

                buffer = xmlC11NNormalizeText(cur->content);
                if (buffer != NULL) {
                    xmlOutputBufferWriteString(ctx->buf,
                                               (const char *) buffer);
                    xmlFree(buffer);
                } else {
                    xmlC14NErrMemory(ctx);
                    return (-1);
                }
            }
            break;
        case XML_PI_NODE:
            /*
             * Processing Instruction (PI) Nodes-
             * The opening PI symbol (<?), the PI target name of the node,
             * a leading space and the string value if it is not empty, and
             * the closing PI symbol (?>). If the string value is empty,
             * then the leading space is not added. Also, a trailing #xA is
             * rendered after the closing PI symbol for PI children of the
             * root node with a lesser document order than the document
             * element, and a leading #xA is rendered before the opening PI
             * symbol of PI children of the root node with a greater document
             * order than the document element.
             */
            if (visible) {
                if (ctx->pos == XMLC14N_AFTER_DOCUMENT_ELEMENT) {
                    xmlOutputBufferWriteString(ctx->buf, "\x0A<?");
                } else {
                    xmlOutputBufferWriteString(ctx->buf, "<?");
                }

                xmlOutputBufferWriteString(ctx->buf,
                                           (const char *) cur->name);
                if ((cur->content != NULL) && (*(cur->content) != '\0')) {
                    xmlChar *buffer;

                    xmlOutputBufferWriteString(ctx->buf, " ");

                    /* todo: do we need to normalize pi? */
                    buffer = xmlC11NNormalizePI(cur->content);
                    if (buffer != NULL) {
                        xmlOutputBufferWriteString(ctx->buf,
                                                   (const char *) buffer);
                        xmlFree(buffer);
                    } else {
                        xmlC14NErrMemory(ctx);
                        return (-1);
                    }
                }

                if (ctx->pos == XMLC14N_BEFORE_DOCUMENT_ELEMENT) {
                    xmlOutputBufferWriteString(ctx->buf, "?>\x0A");
                } else {
                    xmlOutputBufferWriteString(ctx->buf, "?>");
                }
            }
            break;
        case XML_COMMENT_NODE:
            /*
             * Comment Nodes
             * Nothing if generating canonical XML without  comments. For
             * canonical XML with comments, generate the opening comment
             * symbol (<!--), the string value of the node, and the
             * closing comment symbol (-->). Also, a trailing #xA is rendered
             * after the closing comment symbol for comment children of the
             * root node with a lesser document order than the document
             * element, and a leading #xA is rendered before the opening
             * comment symbol of comment children of the root node with a
             * greater document order than the document element. (Comment
             * children of the root node represent comments outside of the
             * top-level document element and outside of the document type
             * declaration).
             */
            if (visible && ctx->with_comments) {
                if (ctx->pos == XMLC14N_AFTER_DOCUMENT_ELEMENT) {
                    xmlOutputBufferWriteString(ctx->buf, "\x0A<!--");
                } else {
                    xmlOutputBufferWriteString(ctx->buf, "<!--");
                }

                if (cur->content != NULL) {
                    xmlChar *buffer;

                    /* todo: do we need to normalize comment? */
                    buffer = xmlC11NNormalizeComment(cur->content);
                    if (buffer != NULL) {
                        xmlOutputBufferWriteString(ctx->buf,
                                                   (const char *) buffer);
                        xmlFree(buffer);
                    } else {
                        xmlC14NErrMemory(ctx);
                        return (-1);
                    }
                }

                if (ctx->pos == XMLC14N_BEFORE_DOCUMENT_ELEMENT) {
                    xmlOutputBufferWriteString(ctx->buf, "-->\x0A");
                } else {
                    xmlOutputBufferWriteString(ctx->buf, "-->");
                }
            }
            break;
        case XML_DOCUMENT_NODE:
        case XML_DOCUMENT_FRAG_NODE:   /* should be processed as document? */
#ifdef LIBXML_HTML_ENABLED
        case XML_HTML_DOCUMENT_NODE:   /* should be processed as document? */
#endif
            if (cur->children != NULL) {
                ctx->pos = XMLC14N_BEFORE_DOCUMENT_ELEMENT;
                ctx->parent_is_doc = 1;
                ret = xmlC14NProcessNodeList(ctx, cur->children);
            }
            break;

        case XML_ATTRIBUTE_NODE:
            xmlC14NErrInvalidNode(ctx, "XML_ATTRIBUTE_NODE", "processing node");
            return (-1);
        case XML_NAMESPACE_DECL:
            xmlC14NErrInvalidNode(ctx, "XML_NAMESPACE_DECL", "processing node");
            return (-1);
        case XML_ENTITY_REF_NODE:
            xmlC14NErrInvalidNode(ctx, "XML_ENTITY_REF_NODE", "processing node");
            return (-1);
        case XML_ENTITY_NODE:
            xmlC14NErrInvalidNode(ctx, "XML_ENTITY_NODE", "processing node");
            return (-1);

        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
#ifdef LIBXML_XINCLUDE_ENABLED
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
#endif
            /*
             * should be ignored according to "W3C Canonical XML"
             */
            break;
        default:
            xmlC14NErrUnknownNode(ctx, cur->type, "processing node");
            return (-1);
    }

    return (ret);
}

/**
 * Processes all nodes in the row starting from cur.
 *
 * @param ctx  		the pointer to C14N context object
 * @param cur  		the node to start from
 * @returns non-negative value on success or negative value on fail
 */
static int
xmlC14NProcessNodeList(xmlC14NCtxPtr ctx, xmlNodePtr cur)
{
    int ret;

    if (ctx == NULL) {
        xmlC14NErrParam(ctx);
        return (-1);
    }

    for (ret = 0; cur != NULL && ret >= 0; cur = cur->next) {
        ret = xmlC14NProcessNode(ctx, cur);
    }
    return (ret);
}


/**
 * Cleanups the C14N context object.
 *
 * @param ctx  the pointer to C14N context object
 */

static void
xmlC14NFreeCtx(xmlC14NCtxPtr ctx)
{
    if (ctx == NULL) {
        xmlC14NErrParam(ctx);
        return;
    }

    if (ctx->ns_rendered != NULL) {
        xmlC14NVisibleNsStackDestroy(ctx->ns_rendered);
    }
    xmlFree(ctx);
}

/**
 * Creates new C14N context object to store C14N parameters.
 *
 * @param doc  		the XML document for canonization
 * @param is_visible_callback  the function to use to determine is node visible
 *			or not
 * @param user_data  		the first parameter for `is_visible_callback` function
 *			(in most cases, it is nodes set)
 * @param mode  the c14n mode (see `xmlC14NMode`)
 * @param inclusive_ns_prefixes  the list of inclusive namespace prefixes
 *			ended with a NULL or NULL if there is no
 *			inclusive namespaces (only for `
 *			canonicalization)
 * @param with_comments  	include comments in the result (!=0) or not (==0)
 * @param buf  		the output buffer to store canonical XML; this
 *			buffer MUST have encoder==NULL because C14N requires
 *			UTF-8 output
 * @returns pointer to newly created object (success) or NULL (fail)
 */
static xmlC14NCtxPtr
xmlC14NNewCtx(xmlDocPtr doc,
	      xmlC14NIsVisibleCallback is_visible_callback, void* user_data,
              xmlC14NMode mode, xmlChar ** inclusive_ns_prefixes,
              int with_comments, xmlOutputBufferPtr buf)
{
    xmlC14NCtxPtr ctx = NULL;

    if ((doc == NULL) || (buf == NULL)) {
        xmlC14NErrParam(ctx);
        return (NULL);
    }

    /*
     *  Validate the encoding output buffer encoding
     */
    if (buf->encoder != NULL) {
        xmlC14NErr(ctx, (xmlNodePtr) doc, XML_C14N_REQUIRES_UTF8,
"xmlC14NNewCtx: output buffer encoder != NULL but C14N requires UTF8 output\n");
        return (NULL);
    }

    /*
     * Allocate a new xmlC14NCtx and fill the fields.
     */
    ctx = (xmlC14NCtxPtr) xmlMalloc(sizeof(xmlC14NCtx));
    if (ctx == NULL) {
	xmlC14NErrMemory(ctx);
        return (NULL);
    }
    memset(ctx, 0, sizeof(xmlC14NCtx));

    /*
     * initialize C14N context
     */
    ctx->doc = doc;
    ctx->with_comments = with_comments;
    ctx->is_visible_callback = is_visible_callback;
    ctx->user_data = user_data;
    ctx->buf = buf;
    ctx->parent_is_doc = 1;
    ctx->pos = XMLC14N_BEFORE_DOCUMENT_ELEMENT;
    ctx->ns_rendered = xmlC14NVisibleNsStackCreate();

    if(ctx->ns_rendered == NULL) {
        xmlC14NErrMemory(ctx);
	xmlC14NFreeCtx(ctx);
        return (NULL);
    }

    /*
     * Set "mode" flag and remember list of inclusive prefixes
     * for exclusive c14n
     */
    ctx->mode = mode;
    if(xmlC14NIsExclusive(ctx)) {
        ctx->inclusive_ns_prefixes = inclusive_ns_prefixes;
    }

    return (ctx);
}

/**
 * Dumps the canonized image of given XML document into the provided buffer.
 * For details see "Canonical XML" (http://www.w3.org/TR/xml-c14n) or
 * "Exclusive XML Canonicalization" (http://www.w3.org/TR/xml-exc-c14n)
 *
 * @param doc  		the XML document for canonization
 * @param is_visible_callback  the function to use to determine is node visible
 *			or not
 * @param user_data  		the first parameter for `is_visible_callback` function
 *			(in most cases, it is nodes set)
 * @param mode  	the c14n mode (see `xmlC14NMode`)
 * @param inclusive_ns_prefixes  the list of inclusive namespace prefixes
 *			ended with a NULL or NULL if there is no
 *			inclusive namespaces (only for exclusive
 *			canonicalization, ignored otherwise)
 * @param with_comments  	include comments in the result (!=0) or not (==0)
 * @param buf  		the output buffer to store canonical XML; this
 *			buffer MUST have encoder==NULL because C14N requires
 *			UTF-8 output
 * @returns non-negative value on success or a negative value on fail
 */
int
xmlC14NExecute(xmlDoc *doc, xmlC14NIsVisibleCallback is_visible_callback,
	 void* user_data, int mode, xmlChar **inclusive_ns_prefixes,
	 int with_comments, xmlOutputBuffer *buf) {

    xmlC14NCtxPtr ctx;
    xmlC14NMode c14n_mode = XML_C14N_1_0;
    int ret;

    if ((buf == NULL) || (doc == NULL)) {
        xmlC14NErrParam(NULL);
        return (-1);
    }

    /* for backward compatibility, we have to have "mode" as "int"
       and here we check that user gives valid value */
    switch(mode) {
    case XML_C14N_1_0:
    case XML_C14N_EXCLUSIVE_1_0:
    case XML_C14N_1_1:
         c14n_mode = (xmlC14NMode)mode;
         break;
    default:
        xmlC14NErrParam(NULL);
        return (-1);
    }

    /*
     *  Validate the encoding output buffer encoding
     */
    if (buf->encoder != NULL) {
        xmlC14NErr(NULL, (xmlNodePtr) doc, XML_C14N_REQUIRES_UTF8,
"xmlC14NExecute: output buffer encoder != NULL but C14N requires UTF8 output\n");
        return (-1);
    }

    ctx = xmlC14NNewCtx(doc, is_visible_callback, user_data,
	            c14n_mode, inclusive_ns_prefixes,
                    with_comments, buf);
    if (ctx == NULL) {
        xmlC14NErr(NULL, (xmlNodePtr) doc, XML_C14N_CREATE_CTXT,
		   "xmlC14NExecute: unable to create C14N context\n");
        return (-1);
    }



    /*
     * Root Node
     * The root node is the parent of the top-level document element. The
     * result of processing each of its child nodes that is in the node-set
     * in document order. The root node does not generate a byte order mark,
     * XML declaration, nor anything from within the document type
     * declaration.
     */
    if (doc->children != NULL) {
        ret = xmlC14NProcessNodeList(ctx, doc->children);
        if (ret < 0) {
            xmlC14NFreeCtx(ctx);
            return (-1);
        }
    }

    /*
     * Flush buffer to get number of bytes written
     */
    ret = xmlOutputBufferFlush(buf);
    if (ret < 0) {
        xmlC14NErr(ctx, NULL, buf->error, "flushing output buffer");
        xmlC14NFreeCtx(ctx);
        return (-1);
    }

    /*
     * Cleanup
     */
    xmlC14NFreeCtx(ctx);
    return (ret);
}

/**
 * Dumps the canonized image of given XML document into the provided buffer.
 * For details see "Canonical XML" (http://www.w3.org/TR/xml-c14n) or
 * "Exclusive XML Canonicalization" (http://www.w3.org/TR/xml-exc-c14n)
 *
 * @param doc  		the XML document for canonization
 * @param nodes  		the nodes set to be included in the canonized image
 *		or NULL if all document nodes should be included
 * @param mode  		the c14n mode (see `xmlC14NMode`)
 * @param inclusive_ns_prefixes  the list of inclusive namespace prefixes
 *			ended with a NULL or NULL if there is no
 *			inclusive namespaces (only for exclusive
 *			canonicalization, ignored otherwise)
 * @param with_comments  	include comments in the result (!=0) or not (==0)
 * @param buf  		the output buffer to store canonical XML; this
 *			buffer MUST have encoder==NULL because C14N requires
 *			UTF-8 output
 * @returns non-negative value on success or a negative value on fail
 */
int
xmlC14NDocSaveTo(xmlDoc *doc, xmlNodeSet *nodes,
                 int mode, xmlChar ** inclusive_ns_prefixes,
                 int with_comments, xmlOutputBuffer *buf) {
    return(xmlC14NExecute(doc,
			xmlC14NIsNodeInNodeset,
			nodes,
			mode,
			inclusive_ns_prefixes,
			with_comments,
			buf));
}


/**
 * Dumps the canonized image of given XML document into memory.
 * For details see "Canonical XML" (http://www.w3.org/TR/xml-c14n) or
 * "Exclusive XML Canonicalization" (http://www.w3.org/TR/xml-exc-c14n)
 *
 * @param doc  		the XML document for canonization
 * @param nodes  		the nodes set to be included in the canonized image
 *		or NULL if all document nodes should be included
 * @param mode  		the c14n mode (see `xmlC14NMode`)
 * @param inclusive_ns_prefixes  the list of inclusive namespace prefixes
 *			ended with a NULL or NULL if there is no
 *			inclusive namespaces (only for exclusive
 *			canonicalization, ignored otherwise)
 * @param with_comments  	include comments in the result (!=0) or not (==0)
 * @param doc_txt_ptr  	the memory pointer for allocated canonical XML text;
 *			the caller of this functions is responsible for calling
 *			xmlFree() to free allocated memory
 * @returns the number of bytes written on success or a negative value on fail
 */
int
xmlC14NDocDumpMemory(xmlDoc *doc, xmlNodeSet *nodes,
                     int mode, xmlChar ** inclusive_ns_prefixes,
                     int with_comments, xmlChar ** doc_txt_ptr)
{
    int ret;
    xmlOutputBufferPtr buf;

    if (doc_txt_ptr == NULL) {
        xmlC14NErrParam(NULL);
        return (-1);
    }

    *doc_txt_ptr = NULL;

    /*
     * create memory buffer with UTF8 (default) encoding
     */
    buf = xmlAllocOutputBuffer(NULL);
    if (buf == NULL) {
        xmlC14NErrMemory(NULL);
        return (-1);
    }

    /*
     * canonize document and write to buffer
     */
    ret = xmlC14NDocSaveTo(doc, nodes, mode, inclusive_ns_prefixes,
                           with_comments, buf);
    if (ret < 0) {
        (void) xmlOutputBufferClose(buf);
        return (-1);
    }

    ret = xmlBufUse(buf->buffer);
    if (ret >= 0) {
        *doc_txt_ptr = xmlStrndup(xmlBufContent(buf->buffer), ret);
    }
    (void) xmlOutputBufferClose(buf);

    if ((*doc_txt_ptr == NULL) && (ret >= 0)) {
        xmlC14NErrMemory(NULL);
        return (-1);
    }
    return (ret);
}

/**
 * Dumps the canonized image of given XML document into the file.
 * For details see "Canonical XML" (http://www.w3.org/TR/xml-c14n) or
 * "Exclusive XML Canonicalization" (http://www.w3.org/TR/xml-exc-c14n)
 *
 * @param doc  		the XML document for canonization
 * @param nodes  		the nodes set to be included in the canonized image
 *		or NULL if all document nodes should be included
 * @param mode  		the c14n mode (see `xmlC14NMode`)
 * @param inclusive_ns_prefixes  the list of inclusive namespace prefixes
 *			ended with a NULL or NULL if there is no
 *			inclusive namespaces (only for exclusive
 *			canonicalization, ignored otherwise)
 * @param with_comments  	include comments in the result (!=0) or not (==0)
 * @param filename  		the filename to store canonical XML image
 * @param compression  	the compression level (zlib required):
 *				-1 - libxml default,
 *				 0 - uncompressed,
 *				>0 - compression level
 * @returns the number of bytes written success or a negative value on fail
 */
int
xmlC14NDocSave(xmlDoc *doc, xmlNodeSet *nodes,
               int mode, xmlChar ** inclusive_ns_prefixes,
               int with_comments, const char *filename, int compression)
{
    xmlOutputBufferPtr buf;
    int ret;

    if (filename == NULL) {
        xmlC14NErrParam(NULL);
        return (-1);
    }
#ifdef LIBXML_ZLIB_ENABLED
    if (compression < 0)
        compression = xmlGetCompressMode();
#endif

    /*
     * save the content to a temp buffer, use default UTF8 encoding.
     */
    buf = xmlOutputBufferCreateFilename(filename, NULL, compression);
    if (buf == NULL) {
        xmlC14NErr(NULL, NULL, XML_IO_UNKNOWN, "creating temporary filename");
        return (-1);
    }

    /*
     * canonize document and write to buffer
     */
    ret = xmlC14NDocSaveTo(doc, nodes, mode, inclusive_ns_prefixes,
                           with_comments, buf);
    if (ret < 0) {
        (void) xmlOutputBufferClose(buf);
        return (-1);
    }

    /*
     * get the numbers of bytes written
     */
    ret = xmlOutputBufferClose(buf);
    return (ret);
}

/**
 * Converts a string to a canonical (normalized) format. The code is stolen
 * from xmlEscapeText. Added normalization of `\x09`, `\x0a`,
 * `\x0A` and the `mode` parameter.
 *
 * @param input  		the input string
 * @param mode  		the normalization mode (attribute, comment, PI or text)
 * @returns a normalized string (caller is responsible for calling #xmlFree)
 * or NULL if an error occurs
 */
static xmlChar *
xmlC11NNormalizeString(const xmlChar * input,
                       xmlC14NNormalizationMode mode)
{
    const xmlChar *cur = input;
    xmlChar *buffer = NULL;
    xmlChar *out = NULL;
    int buffer_size = 0;

    if (input == NULL)
        return (NULL);

    /*
     * allocate an translation buffer.
     */
    buffer_size = 1000;
    buffer = xmlMalloc(buffer_size);
    if (buffer == NULL)
        return (NULL);
    out = buffer;

    while (*cur != '\0') {
        if ((out - buffer) > (buffer_size - 10)) {
            xmlChar *tmp;
            int indx = out - buffer;
            int newSize;

            newSize = xmlGrowCapacity(buffer_size, 1, 1, XML_MAX_ITEMS);
            if (newSize < 0) {
                xmlFree(buffer);
                return(NULL);
            }
            tmp = xmlRealloc(buffer, newSize);
            if (tmp == NULL) {
                xmlFree(buffer);
                return(NULL);
            }
            buffer = tmp;
            buffer_size = newSize;
            out = &buffer[indx];
        }

        if ((*cur == '<') && ((mode == XMLC14N_NORMALIZE_ATTR) ||
                              (mode == XMLC14N_NORMALIZE_TEXT))) {
            *out++ = '&';
            *out++ = 'l';
            *out++ = 't';
            *out++ = ';';
        } else if ((*cur == '>') && (mode == XMLC14N_NORMALIZE_TEXT)) {
            *out++ = '&';
            *out++ = 'g';
            *out++ = 't';
            *out++ = ';';
        } else if ((*cur == '&') && ((mode == XMLC14N_NORMALIZE_ATTR) ||
                                     (mode == XMLC14N_NORMALIZE_TEXT))) {
            *out++ = '&';
            *out++ = 'a';
            *out++ = 'm';
            *out++ = 'p';
            *out++ = ';';
        } else if ((*cur == '"') && (mode == XMLC14N_NORMALIZE_ATTR)) {
            *out++ = '&';
            *out++ = 'q';
            *out++ = 'u';
            *out++ = 'o';
            *out++ = 't';
            *out++ = ';';
        } else if ((*cur == '\x09') && (mode == XMLC14N_NORMALIZE_ATTR)) {
            *out++ = '&';
            *out++ = '#';
            *out++ = 'x';
            *out++ = '9';
            *out++ = ';';
        } else if ((*cur == '\x0A') && (mode == XMLC14N_NORMALIZE_ATTR)) {
            *out++ = '&';
            *out++ = '#';
            *out++ = 'x';
            *out++ = 'A';
            *out++ = ';';
        } else if ((*cur == '\x0D') && ((mode == XMLC14N_NORMALIZE_ATTR) ||
                                        (mode == XMLC14N_NORMALIZE_TEXT) ||
                                        (mode == XMLC14N_NORMALIZE_COMMENT) ||
					(mode == XMLC14N_NORMALIZE_PI))) {
            *out++ = '&';
            *out++ = '#';
            *out++ = 'x';
            *out++ = 'D';
            *out++ = ';';
        } else {
            /*
             * Works because on UTF-8, all extended sequences cannot
             * result in bytes in the ASCII range.
             */
            *out++ = *cur;
        }
        cur++;
    }
    *out = 0;
    return (buffer);
}

#endif /* LIBXML_C14N_ENABLED */
