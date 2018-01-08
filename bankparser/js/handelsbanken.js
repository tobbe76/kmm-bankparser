
function parseAccounts() {
    var accDiv = document.getElementById("account_cards_m");
    var tables = accDiv.getElementsByTagName("table");
    var rows = tables[0].tBodies[0].rows;
    var accArr = [];
    for(var i = 0; i < rows.length; i++) {
        var href = rows[i].cells[0].getElementsByTagName("a")[0].href;
        var number = rows[i].cells[0].getElementsByTagName("span")[0].innerHTML.split('-')[1].trim();
        accArr.push({Link:href, Name:rows[i].cells[0].innerText.trim(), Number:number});
    }
    jshelper.parseAccountTables(accArr);
}

function handleStatements() {
    var inside = bpIframeRef(document.getElementById("my-frame"));
    var accDiv = inside.getElementsByTagName("form");
    var transArr = [];

    for(var j = 0; j < accDiv.length; j++) {
        if("accounttransactions" == accDiv[j].getAttribute("name")) {
            var tables = accDiv[j].getElementsByTagName("table");
            var rows = tables[12].tBodies[0].rows;
            for(var i = 2; i < rows.length; i++) {
                var trans = rows[i];
                var date = trans.cells[2].innerText;
                var name = trans.cells[3].innerText;
                var sum  = trans.cells[4].innerText;
                var extra = "Empty";
                transArr.push({Name:name, Date:date, Sum:sum, Extra:extra});
            }
        }
    }
    jshelper.parseStatements(transArr);
}

function parseStatements() {
    bpWaitTagInIframe(document.getElementById("my-frame"), "form", handleStatements);
}
