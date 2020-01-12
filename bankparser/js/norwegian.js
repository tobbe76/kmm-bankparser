var newestYear = 0;
var oldestYear = 0;
var accArr = [];
var first = 0;

function handleStatements() {
    var accDiv = document.getElementById("transactions");
    var tables = accDiv.getElementsByTagName("table");
    accArr = [];
    var bodies = tables[0].tBodies;
    console.log("handleStatements " + bodies.length);
    for(var i = 0; i < bodies.length; i++) {
        var trans = bodies[i];
        var date = trans.rows[0].cells[0].innerText;
        var name = trans.rows[0].cells[1].getElementsByTagName("div")[0].innerText;
        var type = trans.rows[0].cells[1].getElementsByTagName("div")[1].innerText;
        var sum  = trans.rows[0].cells[4].innerText;
        console.log("Push " + name + " " + date);
        accArr.push({Name:name, Date:date, Sum:sum, Extra:type});
    }
    if(newestYear >= oldestYear) {
        selectYear(newestYear);
    }
    else {
        jshelper.parseStatementsResponse(accArr);
    }
}

function selectYear(year) {
    console.log("Select year " + year.toString());
    document.getElementsByName("YearList")[0].value = year.toString();
    var event = document.createEvent('Event');
    event.initEvent('change', true, true);
    document.getElementsByName("YearList")[0].dispatchEvent(event);
    newestYear--;
}

function selectYearPending() {
    console.log("selectYearPending");

    if(first == 1) {
        first = 0;
        $( document ).ajaxStop(handleStatements);
        selectYear(newestYear);
    }
}

function hoho() {
    console.log("hoho");



    var epa = document.getElementsByName("PeriodSelect");
    console.log("APA " + epa.toString());
    document.getElementsByName("PeriodSelect")[0].value = '2';
    var event = document.createEvent('Event');
    event.initEvent('change', true, true);
    document.getElementsByName("PeriodSelect")[0].dispatchEvent(event);

    setTimeout(handleStatements, 1000)

}

function parseAccounts() {
    console.log("parseAccounts2");
    var data = document.getElementsByTagName("div")[0].getAttribute("data-settings")
    var json = JSON.parse(data);

    var number = json.pageModels.appPage.customModel.accountNo;

    console.log("push account" + number);
    accArr.push({Link:"", Name:"Creditcard", Number:number});
    jshelper.parseAccountsResponse(accArr);
}

function parseStatements() {
    console.log("parseStatements");
    accArr = [];

    var json = JSON.parse(document.body.innerText);
    json.forEach(function(trans){
        var name = trans.transactionText;
        var date = trans.transactionDate.substring(0, 10);
        var sum = trans.amountFormatted;
        var type = trans.transactionTypeText;
        console.log("trans " + name + " " + date);

        accArr.push({Name:name, Date:date, Sum:sum, Extra:type});

    });
    jshelper.parseStatementsResponse(accArr);
}
