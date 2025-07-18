#ifndef XML_ENC_H_PRIVATE__
#define XML_ENC_H_PRIVATE__

#include "../encoding.h"
#include "../tree.h"

XML_HIDDEN void
xmlInitEncodingInternal(void);

XML_HIDDEN xmlCharEncError
xmlEncInputChunk(xmlCharEncodingHandler *handler, unsigned char *out,
                 int *outlen, const unsigned char *in, int *inlen,
                 int flush);
XML_HIDDEN xmlCharEncError
xmlCharEncInput(xmlParserInputBuffer *input, size_t *sizeOut, int flush);
XML_HIDDEN int
xmlCharEncOutput(xmlOutputBuffer *output, int init);

#endif /* XML_ENC_H_PRIVATE__ */
