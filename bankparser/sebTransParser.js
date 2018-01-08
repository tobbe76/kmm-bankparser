
function func() {
    Sys.WebForms.PageRequestManager.getInstance().add_endRequest( SebParser.newPageLoadedSignal );
    var updSearch  = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_UpdSearch");
    var butts = updSearch.getElementsByClassName("m-button")
    var newestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T1");
    var oldestDate = document.getElementById("IKPMaster_MainPlaceHolder_ucAccountEvents_T2");
    newestDate.value = "%1";
    oldestDate.value = "%2";
    var event = document.createEvent('Event');
    event.initEvent('click', true, true);
    butts[2].dispatchEvent(event);
}
