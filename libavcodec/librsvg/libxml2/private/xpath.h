#ifndef XML_XPATH_H_PRIVATE__
#define XML_XPATH_H_PRIVATE__

#include "../xpath.h"

XML_HIDDEN void
xmlInitXPathInternal(void);

#ifdef LIBXML_XPATH_ENABLED
XML_HIDDEN void
xmlXPathErrMemory(xmlXPathContext *ctxt);
XML_HIDDEN void
xmlXPathPErrMemory(xmlXPathParserContext *ctxt);
#endif

#endif /* XML_XPATH_H_PRIVATE__ */
