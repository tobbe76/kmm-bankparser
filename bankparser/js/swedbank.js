var mutationObserver;

function accountHandler(mutations) {
    console.log("accountHandler");
    var accList = document.getElementsByClassName("swed-ui-list__body");
    if(accList.length > 1) {
        var allLinks = document.getElementsByTagName("swed-item");
        var accArr = [];
        var accLists = document.getElementsByClassName("swed-ui-list__body");
        for(var i = 0; i < accLists.length; i++) {
            var infoLists = accLists[i].getElementsByTagName("li");
            for(var j = 0; j < infoLists.length; j++) {
                var nameEl = infoLists[j].getElementsByTagName("sw-label");
                var nrEl = infoLists[j].getElementsByTagName("sw-label-detail");
                var linkIndex = 0;
                var link = infoLists[j].getElementsByTagName("swed-item");
                for(var k = 0; k < allLinks.length; k++) {
                    if(allLinks[k] == link[0]) {
                        linkIndex = k;
                    }
                }
                if(nrEl.length > 0 && nameEl.length > 0) {
                    accArr.push({Name:nameEl[0].innerText, Number:nrEl[0].innerText, LinkIndex:linkIndex});
                }
            }
        }
        jshelper.parseAccountTables(accArr);
        mutationObserver.disconnect();
    }
}

function parseAccounts() {
    console.log("parseAccounts");
    mutationObserver = new MutationObserver(accountHandler);

    mutationObserver.observe(document, bpMutDef);
    var sidebar = document.getElementById("sidebarArea");
    var butts = sidebar.getElementsByTagName("a");
    bpClick(butts[1]);
}

function statementHandler(mutations) {
    console.log("statementHandler")
    var tables = document.getElementsByClassName("account-table__table");
    if(tables.length > 0) {
        var transArr = [];
        var rows = tables[0].tBodies[0].rows;
        for(var i = 2; i < rows.length; i++) {
            var divs = rows[i].cells[0].getElementsByTagName("div");
            var name = divs[1].innerText;
            var date = divs[2].innerText;
            var divs2 = rows[i].cells[2].getElementsByTagName("div");
            var sum = divs2[1].innerText;
            transArr.push({Name:name, Date:date, Sum:sum});
        }
        jshelper.parseStatements(transArr);
        mutationObserver.disconnect();
    }
}

function parseStatements(accountIndex) {
    mutationObserver = new MutationObserver(statementHandler);

    mutationObserver.observe(document, bpMutDef);
    var allLinks = document.getElementsByTagName("swed-item");
    bpClick(allLinks[accountIndex]);
}
