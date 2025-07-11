#include <stddef.h>
#include "resources.h"

const unsigned char *ff_graph_html_data = "\
FFmpeg Execution Graph\n\
__###__";

const unsigned char *ff_graph_css_data = "\
/* Variables */\n\
.root {\n\
    --ff-colvideo: #6eaa7b;\n\
    --ff-colaudio: #477fb3;\n\
    --ff-colsubtitle: #ad76ab;\n\
    --ff-coltext: #666;\n\
}\n\
\n\
/* Common & Misc */\n\
.ff-inputfiles rect, .ff-outputfiles rect, .ff-inputstreams rect, .ff-outputstreams rect, .ff-decoders rect, .ff-encoders rect {\n\
    stroke-width: 0;\n\
    stroke: transparent;\n\
    filter: none !important;\n\
    fill: transparent !important;\n\
    display: none !important;\n\
}\n\
\n\
.cluster span {\n\
    color: var(--ff-coltext);\n\
}\n\
\n\
.cluster rect {\n\
    stroke: #dfdfdf !important;\n\
    transform: translateY(-2.3rem);\n\
    filter: drop-shadow(1px 2px 2px rgba(185,185,185,0.2)) !important;\n\
    rx: 8;\n\
    ry: 8;\n\
}\n\
\n\
.cluster-label {\n\
    font-size: 1.1rem;\n\
}\n\
\n\
    .cluster-label .nodeLabel {\n\
        display: block;\n\
        font-weight: 500;\n\
        color: var(--ff-coltext);\n\
    }\n\
\n\
    .cluster-label div {\n\
        max-width: unset !important;\n\
        padding: 3px;\n\
    }\n\
\n\
    .cluster-label foreignObject {\n\
        transform: translateY(-0.7rem);\n\
    }\n\
\n\
/* Input and output files */\n\
.node.ff-inputfile .label foreignObject, .node.ff-outputfile .label foreignObject {\n\
    overflow: visible;\n\
}\n\
\n\
.cluster.ff-inputfile .cluster-label foreignObject div:not(foreignObject div div), .cluster.ff-outputfile .cluster-label foreignObject div:not(foreignObject div div) {\n\
    display: table !important;\n\
}\n\
\n\
.nodeLabel div.ff-inputfile, .nodeLabel div.ff-outputfile {\n\
    font-size: 1.1rem;\n\
    font-weight: 500;\n\
    min-width: 14rem;\n\
    width: 100%;\n\
    display: flex;\n\
    color: var(--ff-coltext);\n\
    margin-top: 0.1rem;\n\
    line-height: 1.35;\n\
    padding-bottom: 1.9rem;\n\
}\n\
\n\
.nodeLabel div.ff-outputfile {\n\
    flex-direction: row-reverse;\n\
}\n\
\n\
.ff-inputfile .index, .ff-outputfile .index {\n\
    order: 2;\n\
    color: var(--ff-coltext);\n\
    text-align: center;\n\
    border-radius: 0.45rem;\n\
    border: 0.18em solid #666666db;\n\
    font-weight: 600;\n\
    padding: 0 0.3em;\n\
    opacity: 0.8;\n\
}\n\
\n\
    .ff-inputfile .index::before {\n\
        content: 'In ';\n\
    }\n\
\n\
    .ff-outputfile .index::before {\n\
        content: 'Out ';\n\
    }\n\
\n\
.ff-inputfile .demuxer_name, .ff-outputfile .muxer_name {\n\
    flex: 1;\n\
    order: 1;\n\
    font-size: 0.9rem;\n\
    white-space: nowrap;\n\
    overflow: hidden;\n\
    text-overflow: ellipsis;\n\
    text-align: center;\n\
    max-width: 8rem;\n\
    align-content: center;\n\
    margin: 0.2rem 0.4rem 0 0.4rem;\n\
}\n\
\n\
.ff-inputfile .file_extension, .ff-outputfile .file_extension {\n\
    order: 0;\n\
    background-color: #888;\n\
    color: white;\n\
    text-align: center;\n\
    border-radius: 0.45rem;\n\
    font-weight: 600;\n\
    padding: 0 0.4em;\n\
    align-content: center;\n\
    opacity: 0.8;\n\
}\n\
\n\
.ff-inputfile .url, .ff-outputfile .url {\n\
    order: 4;\n\
    text-align: center;\n\
    position: absolute;\n\
    left: 0;\n\
    right: 0;\n\
    bottom: 0.75rem;\n\
    font-size: 0.7rem;\n\
    font-weight: 400;\n\
    white-space: nowrap;\n\
    overflow: hidden;\n\
    text-overflow: ellipsis;\n\
    margin: 0 0.3rem;\n\
    direction: rtl;\n\
    color: #999;\n\
}\n\
\n\
.cluster.ff-inputfile rect, .cluster.ff-outputfile rect {\n\
    transform: translateY(-1.8rem);\n\
    fill: url(#ff-radgradient);\n\
}\n\
\n\
/* Input and output streams */\n\
.node.ff-inputstream rect, .node.ff-outputstream rect {\n\
    padding: 0 !important;\n\
    margin: 0 !important;\n\
    border: none !important;\n\
    fill: white;\n\
    stroke: #e5e5e5 !important;\n\
    height: 2.7rem;\n\
    transform: translateY(0.2rem);\n\
    filter: none;\n\
    rx: 3;\n\
    ry: 3;\n\
}\n\
\n\
.node.ff-inputstream .label foreignObject, .node.ff-outputstream .label foreignObject {\n\
    transform: translateY(-0.2%);\n\
    overflow: visible;\n\
}\n\
\n\
    .node.ff-inputstream .label foreignObject div:not(foreignObject div div), .node.ff-outputstream .label foreignObject div:not(foreignObject div div) {\n\
        display: block !important;\n\
        line-height: 1.5 !important;\n\
    }\n\
\n\
.nodeLabel div.ff-inputstream, .nodeLabel div.ff-outputstream {\n\
    font-size: 1.0rem;\n\
    font-weight: 500;\n\
    min-width: 12rem;\n\
    width: 100%;\n\
    display: flex;\n\
}\n\
\n\
.nodeLabel div.ff-outputstream {\n\
    flex-direction: row-reverse;\n\
}\n\
\n\
.ff-inputstream .name, .ff-outputstream .name {\n\
    flex: 1;\n\
    white-space: nowrap;\n\
    overflow: hidden;\n\
    text-overflow: ellipsis;\n\
    text-align: left;\n\
    align-content: center;\n\
    margin-bottom: -0.15rem;\n\
}\n\
\n\
.ff-inputstream .index, .ff-outputstream .index {\n\
    flex: 0 0 1.4rem;\n\
    background-color: #888;\n\
    color: white;\n\
    text-align: center;\n\
    border-radius: 0.3rem;\n\
    font-weight: 600;\n\
    margin-right: -0.3rem;\n\
    margin-left: 0.4rem;\n\
    opacity: 0.8;\n\
}\n\
\n\
.ff-outputstream .index {\n\
    margin-right: 0.6rem;\n\
    margin-left: -0.4rem;\n\
}\n\
\n\
.ff-inputstream::before, .ff-outputstream::before {\n\
    font-variant-emoji: text;\n\
    flex: 0 0 2rem;\n\
    margin-left: -0.8rem;\n\
    margin-right: 0.2rem;\n\
}\n\
\n\
.ff-outputstream::before {\n\
    margin-left: 0.2rem;\n\
    margin-right: -0.6rem;\n\
}\n\
\n\
.ff-inputstream.video::before, .ff-outputstream.video::before {\n\
    content: '\239A';\n\
    color: var(--ff-colvideo);\n\
    font-size: 2.25rem;\n\
    line-height: 0.5;\n\
    font-weight: bold;\n\
}\n\
\n\
.ff-inputstream.audio::before, .ff-outputstream.audio::before {\n\
    content: '\1F39D';\n\
    color: var(--ff-colaudio);\n\
    font-size: 1.75rem;\n\
    line-height: 0.9;\n\
}\n\
\n\
.ff-inputstream.subtitle::before, .ff-outputstream.subtitle::before {\n\
    content: '\1AC';\n\
    color: var(--ff-colsubtitle);\n\
    font-size: 1.2rem;\n\
    line-height: 1.1;\n\
    transform: scaleX(1.5);\n\
    margin-top: 0.050rem;\n\
}\n\
\n\
.ff-inputstream.attachment::before, .ff-outputstream.attachment::before {\n\
    content: '\1F4CE';\n\
    font-size: 1.3rem;\n\
    line-height: 1.15;\n\
}\n\
\n\
.ff-inputstream.data::before, .ff-outputstream.data::before {\n\
    content: '\27E8\2219\2219\2219\27E9';\n\
    font-size: 1.15rem;\n\
    line-height: 1.17;\n\
    letter-spacing: -0.3px;\n\
}\n\
\n\
/* Filter Graphs */\n\
.cluster.ff-filters rect {\n\
    stroke-dasharray: 6 !important;\n\
    stroke-width: 1.3px;\n\
    stroke: #d1d1d1 !important;\n\
    filter: none !important;\n\
}\n\
\n\
.cluster.ff-filters div.ff-filters .id {\n\
    display: none;\n\
}\n\
\n\
.cluster.ff-filters div.ff-filters .name {\n\
    margin-right: 0.5rem;\n\
    font-size: 0.9rem;\n\
}\n\
\n\
.cluster.ff-filters div.ff-filters .description {\n\
    font-weight: 400;\n\
    font-size: 0.75rem;\n\
    vertical-align: middle;\n\
    color: #777;\n\
    font-family: Cascadia Code, Lucida Console, monospace;\n\
}\n\
\n\
/* Filter Shapes */\n\
.node.ff-filter rect {\n\
    rx: 10;\n\
    ry: 10;\n\
    stroke-width: 1px;\n\
    stroke: #d3d3d3;\n\
    fill: url(#ff-filtergradient);\n\
    filter: drop-shadow(1px 1px 2px rgba(0, 0, 0, 0.1));\n\
}\n\
\n\
.node.ff-filter .label foreignObject {\n\
    transform: translateY(-0.4rem);\n\
    overflow: visible;\n\
}\n\
\n\
.nodeLabel div.ff-filter {\n\
    font-size: 1.0rem;\n\
    font-weight: 500;\n\
    text-transform: uppercase;\n\
    min-width: 5.5rem;\n\
    margin-bottom: 0.5rem;\n\
}\n\
\n\
    .nodeLabel div.ff-filter span {\n\
        color: inherit;\n\
    }\n\
\n\
/* Decoders & Encoders */\n\
.node.ff-decoder rect, .node.ff-encoder rect {\n\
    stroke-width: 1px;\n\
    stroke: #d3d3d3;\n\
    fill: url(#ff-filtergradient);\n\
    filter: drop-shadow(1px 1px 2px rgba(0, 0, 0, 0.1));\n\
}\n\
\n\
.nodeLabel div.ff-decoder, .nodeLabel div.ff-encoder {\n\
    font-size: 0.85rem;\n\
    font-weight: 500;\n\
    min-width: 3.5rem;\n\
}\n\
\n\
/* Links and Arrows */\n\
path.flowchart-link[id|='video'] {\n\
    stroke: var(--ff-colvideo);\n\
}\n\
\n\
path.flowchart-link[id|='audio'] {\n\
    stroke: var(--ff-colaudio);\n\
}\n\
\n\
path.flowchart-link[id|='subtitle'] {\n\
    stroke: var(--ff-colsubtitle);\n\
}\n\
\n\
marker.marker path {\n\
    fill: context-stroke;\n\
}\n\
\n\
.edgeLabel foreignObject {\n\
    transform: translateY(-1rem);\n\
}\n\
\n\
.edgeLabel p {\n\
    background: transparent;\n\
    white-space: nowrap;\n\
    margin: 1rem 0.5rem !important;\n\
    font-weight: 500;\n\
    color: var(--ff-coltext);\n\
}\n\
\n\
.edgeLabel, .labelBkg {\n\
    background: transparent;\n\
}\n\
\n\
.edgeLabels .edgeLabel * {\n\
    font-size: 0.8rem;\n\
}";
