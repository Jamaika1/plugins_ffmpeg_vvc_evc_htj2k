/*
 * Summary: the XML document serializer
 * Description: API to save document or subtree of document
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_XMLSAVE_H__
#define __XML_XMLSAVE_H__

#include "xmlversion.h"
#include "tree.h"
#include "encoding.h"
#include "xmlIO.h"

#ifdef LIBXML_OUTPUT_ENABLED
#ifdef __cplusplus
extern "C" {
#endif

/**
 * xmlSaveOption:
 *
 * This is the set of XML save options that can be passed down
 * to the xmlSaveToFd() and similar calls.
 */
typedef enum {
    XML_SAVE_FORMAT     = 1<<0,	/* format save output */
    XML_SAVE_NO_DECL    = 1<<1,	/* drop the xml declaration */
    XML_SAVE_NO_EMPTY	= 1<<2, /* no empty tags */
    XML_SAVE_NO_XHTML	= 1<<3, /* disable XHTML1 specific rules */
    XML_SAVE_XHTML	= 1<<4, /* force XHTML1 specific rules */
    XML_SAVE_AS_XML     = 1<<5, /* force XML serialization on HTML doc */
    XML_SAVE_AS_HTML    = 1<<6, /* force HTML serialization on XML doc */
    XML_SAVE_WSNONSIG   = 1<<7  /* format with non-significant whitespace */
} xmlSaveOption;


typedef struct _xmlSaveCtxt xmlSaveCtxt;
typedef xmlSaveCtxt *xmlSaveCtxtPtr;

/** DOC_DISABLE */
#define XML_GLOBALS_SAVE \
  XML_OP(xmlIndentTreeOutput, int, XML_EMPTY) \
  XML_OP(xmlTreeIndentString, const char *, XML_EMPTY) \
  XML_OP(xmlSaveNoEmptyTags, int, XML_EMPTY)

#define XML_OP XML_DECLARE_GLOBAL
XML_GLOBALS_SAVE
#undef XML_OP

#if defined(LIBXML_THREAD_ENABLED) && !defined(XML_GLOBALS_NO_REDEFINITION)
  #define xmlIndentTreeOutput XML_GLOBAL_MACRO(xmlIndentTreeOutput)
  #define xmlTreeIndentString XML_GLOBAL_MACRO(xmlTreeIndentString)
  #define xmlSaveNoEmptyTags XML_GLOBAL_MACRO(xmlSaveNoEmptyTags)
#endif
/** DOC_ENABLE */

XMLPUBFUN xmlSaveCtxtPtr
		xmlSaveToFd		(int fd,
					 const char *encoding,
					 int options);
XMLPUBFUN xmlSaveCtxtPtr
		xmlSaveToFilename	(const char *filename,
					 const char *encoding,
					 int options);

XMLPUBFUN xmlSaveCtxtPtr
		xmlSaveToBuffer		(xmlBufferPtr buffer,
					 const char *encoding,
					 int options);

XMLPUBFUN xmlSaveCtxtPtr
		xmlSaveToIO		(xmlOutputWriteCallback iowrite,
					 xmlOutputCloseCallback ioclose,
					 void *ioctx,
					 const char *encoding,
					 int options);

XMLPUBFUN long
		xmlSaveDoc		(xmlSaveCtxtPtr ctxt,
					 xmlDocPtr doc);
XMLPUBFUN long
		xmlSaveTree		(xmlSaveCtxtPtr ctxt,
					 xmlNodePtr node);

XMLPUBFUN int
		xmlSaveFlush		(xmlSaveCtxtPtr ctxt);
XMLPUBFUN int
		xmlSaveClose		(xmlSaveCtxtPtr ctxt);
XMLPUBFUN int
		xmlSaveSetEscape	(xmlSaveCtxtPtr ctxt,
					 xmlCharEncodingOutputFunc escape);
XMLPUBFUN int
		xmlSaveSetAttrEscape	(xmlSaveCtxtPtr ctxt,
					 xmlCharEncodingOutputFunc escape);

XMLPUBFUN int
                xmlThrDefIndentTreeOutput(int v);
XMLPUBFUN const char *
                xmlThrDefTreeIndentString(const char * v);
XMLPUBFUN int
                xmlThrDefSaveNoEmptyTags(int v);

#ifdef __cplusplus
}
#endif

#else /* LIBXML_OUTPUT_ENABLED */

/** DOC_DISABLE */
#define XML_GLOBALS_SAVE
/** DOC_ENABLE */

#endif /* LIBXML_OUTPUT_ENABLED */
#endif /* __XML_XMLSAVE_H__ */


