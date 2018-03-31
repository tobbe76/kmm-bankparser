



var transToExpandArr = [];
var transArr = [];

function handleStatements() {
    console.log("handleStatements ");

    var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
    var tables = accDiv.getElementsByTagName("table");
    var rows = tables[0].tBodies[0].rows;
    for(var i = 0; i < rows.length; i = i +2) {
        var trans = rows[i];
        if(trans.cells.length > 4) {
            var date = trans.cells[1].innerText;
            var name = trans.cells[2].innerText.split('/')[0];
            var sum  = trans.cells[4].innerText;
            var balance = trans.cells[5].innerText;
            if(sum.length == 0) {
                sum = trans.cells[3].innerText;
            }
            var extra = "";
            var valList = rows[i+1].cells[1].getElementsByClassName("key-value-list");
            for(var j = 0; j < valList.length; j = j +1) {
                extra += valList[j].innerText;
            }
            transArr.push({Name:name, Date:date, Sum:sum, Extra:extra, Balance:balance});
        }
    }
    var nextButton = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_BTN_NEXT");
    if(nextButton) {
        console.log("click next" + nextButton.outerHTML);
        Sys.WebForms.PageRequestManager.getInstance().add_endRequest(findStatementsToExpand);
        document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_BTN_NEXT").click();
    }
    else {
        console.log("finsished");
        jshelper.parseStatementsResponse(transArr);
    }
}

function expandStatement() {
    console.log("expandStatement " +  transToExpandArr.length);

    if(transToExpandArr.length == 0) {
        Sys.WebForms.PageRequestManager.getInstance().remove_endRequest(expandStatement);
        handleStatements();
    }
    else {
        var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
        var tables = accDiv.getElementsByTagName("table");
        var rows = tables[0].tBodies[0].rows;
        bpClick(rows[transToExpandArr.pop()]);
    }
}

function findStatementsToExpand() {
    console.log("findStatementsToExpand");

    Sys.WebForms.PageRequestManager.getInstance().remove_endRequest(findStatementsToExpand);
    transToExpandArr = [];
    var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable");
    if(accDiv !== null) {
        var tables = accDiv.getElementsByTagName("table");
        var rows = tables[0].tBodies[0].rows;
        for(var i = 2; i < rows.length; i = i +2) {
            if(rows[i+1].cells[1].getElementsByClassName("key-value-list").length == 0) {
                transToExpandArr.push(i);
            }
        }
        Sys.WebForms.PageRequestManager.getInstance().add_endRequest(expandStatement);
        expandStatement();
    }
    else { //No transactions return empty
        jshelper.parseStatementsResponse(transArr);
    }
}

function parseStatements(nDate, oDate) {
    transArr = [];
    console.log("parseStatements " + nDate + " " + oDate);
    Sys.WebForms.PageRequestManager.getInstance().add_endRequest(findStatementsToExpand);
    var updSearch  = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_UpdSearch");
    var butts = updSearch.getElementsByClassName("m-button--primary")
    var newestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T1");
    var oldestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T2");
    newestDate.value = nDate;
    oldestDate.value = oDate;
    bpClick(butts[0]);
    null;
}


function parseAccounts() {
    console.log("parseAccounts");
    var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountTable_pnlAccounts");
    var tables = accDiv.getElementsByTagName("table");
    var accArr = [];
    for(var i = 0; i < tables.length; i++) {
        var tab = tables[i].tBodies[0];
        for(var j = 0; j < tab.rows.length; j++) {
            var href = tab.rows[j].cells[0].getElementsByTagName("a")[0].href;
            console.log("push account");
            accArr.push({Link:href, Name:tab.rows[j].cells[0].innerText.trim(), Number:tab.rows[j].cells[1].innerText.trim()});
        }
    }
    jshelper.parseAccountsResponse(accArr);
    null;
}

function isLoggedIn() {
    console.log("isLoggedIn");
    var accDiv = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountTable_pnlAccounts");
    var res = false;
    if(accDiv) {
        console.log("isLoggedIn accdiv found");

        res = true;
    } else {
        console.log("isLoggedIn accdiv NOT found  " + accDiv);

    }

    jshelper.isLoggedInResponse(res);
    null;
}
