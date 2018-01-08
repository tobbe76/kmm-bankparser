

var jshelper;
var bpWebChan;

function bpIframeRef( frameRef ) {
    return frameRef.contentWindow
        ? frameRef.contentWindow.document
        : frameRef.contentDocument
}

//Poll for loaded iframe
function bpWaitTagInIframe(iframe, tag, fn)
{
    var inside = bpIframeRef(iframe);
    var accDiv = inside.getElementsByTagName(tag);
    if(accDiv.length > 0) {
        handleStatements();
        return;
    }

    setTimeout(bpWaitTagInIframe, 40, iframe, tag, fn);
}

function bpInitWebChannel() {
    console.log("INITIALIZE WEBCHANNEL");
    bpWebChan = new QWebChannel(qt.webChannelTransport, function (channel) {
        jshelper = channel.objects.jshelper;
        jshelper.webChannelInitialized();
    });
}

function bpClick(el) {
    var event = document.createEvent('Event');
    event.initEvent('click', true, true);
    el.dispatchEvent(event);
}

var bpMutDef = { childList: true,
    subtree: true };

bpInitWebChannel();
