
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
    if(first == 1) {
        first = 0;
        $( document ).ajaxStop(handleStatements);
        selectYear(newestYear);
    }
}

function parseStatements(nYear, oYear) {
    console.log("parseStatements");
    first = 1;
    newestYear = nYear;
    oldestYear = oYear;
    $( document ).ajaxStop(selectYearPending);
}
