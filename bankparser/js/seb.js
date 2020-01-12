



var transToExpandArr = [];
var transArr = [];
var g_nDate;
var g_oDate;

var decodeHtmlEntity = function(str) {
  return str.replace(/&#(\d+);/g, function(match, dec) {
    return String.fromCharCode(dec);
  });
};

function getExtraString(j) {
    extraStr = "";
    var keys = Object.keys(j);
    for(var i=0; i<keys.length; i++){
        if(typeof j[keys[i]] === "object") {
            extraStr += getExtraString(j[keys[i]])
        }
        else {
            if(String(j[keys[i]]).length > 0) {
                extraStr += keys[i] + ":" + j[keys[i]] + "\n";
            }
        }
    }
    return extraStr;
}

function handleStatements() {
    console.log("handleStatements ");

    console.log(this.responseText);

    var jsonData = JSON.parse(this.responseText);
    for(var i = 0; i < jsonData.account_transactions.length; i++) {
        var trans = jsonData.account_transactions[i];
        var date = trans.posting_date;
        var name = trans.descriptive_text;
        var sum = String(trans.transaction_amount.amount).replace(".", ",");
        var balance = String(trans.balance_after_transaction).replace(".", ",");
        var extra = getExtraString(trans);

        if(trans.has_details) {
            var id = trans.id;
            id = id.replace("=", "%3D");
            id = id.replace("=", "%3D");
            id = id.replace("=", "%3D");

            var base = "https://privat.ib.seb.se/wow/microsite/accountsweb/_api/base/api";
            var hash = window.location.hash;
            var url = base + hash.substr(1) + "?transactionId=" + id;
            console.log(url);
            xmlhttp=new XMLHttpRequest();
            xmlhttp.open("GET", url, false);
            xmlhttp.send();
            console.log(xmlhttp.responseText);

            var data = JSON.parse(xmlhttp.responseText);
            extra += "\n" + getExtraString(data.data);
            name = data.data.descriptive_text;
            if(data.data.transaction_type.text === "Swishbetalning") {
                console.log("SWISH");
                if(parseInt(sum) > 0) {
                    name = data.data.additional_info.sender_name;
                }
                else {
                    name = data.data.additional_info.receiver_name;
                }
            }
        }
        transArr.push({Name:decodeHtmlEntity(name), Date:date, Sum:sum, Extra:extra, Balance:balance});
        console.log(date + " " + sum + " " + name + " " + balance)
    }
    console.log("finsished");
    jshelper.parseStatementsResponse(transArr);
}

function parseStatements2(nDate, oDate) {
    var base = "https://privat.ib.seb.se/wow/microsite/accountsweb/_api/base/api";
    var hash = window.location.hash;
    var url = base + hash.substr(1) + "/search";
    console.log("Post url: " + url);
    xhr=new XMLHttpRequest();
    xhr.addEventListener("load", handleStatements);
    xhr.open('POST', url, true)
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    var sn = oDate.split("-");
    var so = nDate.split("-");

    dd = {"filters":{"date":{"start":{"year":parseInt(sn[0]),"month":parseInt(sn[1]),"day":parseInt(sn[2])},
                               "end":{"year":parseInt(so[0]),"month":parseInt(so[1]),"day":parseInt(so[2])}},
          "type":0},
          "maxRows":100,"pagingCursor":null};
    console.log("Post data: " + JSON.stringify(dd));
    xhr.send(JSON.stringify(dd));
    null;
}

function parseStatements(nDate, oDate) {
    setTimeout(parseStatements2, 500, nDate, oDate);
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
