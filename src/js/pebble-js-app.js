/**********************************
    *** Created by: Mongo527 ***
**********************************/

var config;

var BASE_URL;
var options;

var index = 0;

var upcomingTypeCount = 0;
var upcoming = new Array();
var time = new Array();

Pebble.addEventListener("ready", function(e) {
    console.log("App is Ready!");
    console.log(e.type);
});

Pebble.addEventListener("showConfiguration", function() {
    console.log("Showing Configuration");
    config = localStorage.getItem("pebble-sickbeard-config");
    Pebble.openURL('http://shawnconroyd.com/pebble/sb-config.html?' + encodeURIComponent(config));
});

Pebble.addEventListener("webviewclosed", function(e) {
    console.log("Configuration Closed");
    // webview closed
    options = JSON.parse(decodeURIComponent(e.response));
    //console.log("Options = " + JSON.stringify(options));
    localStorage.setItem("pebble-sickbeard-config", JSON.stringify(options));
    BASE_URL = "http://" + options.serverAddress + ":" + options.serverPort + "/api/" + options.apikey + "/?cmd=";
});

function sendNextMessage(type, messages, subtitles) {    
    if (messages.length === 0) {
        index = 0;
        return;
    }
    
    var json;
    
    var message = messages.shift();
    var subtitle = subtitles.shift();
    //console.log("Index: " + index + " - Show: " + show);
    
    if(type == "shows") {
        json = {"sb_shows": message, "shows_status": subtitle, "shows_index": index};
    }
    
    else if(type == "upcoming") {
        json = {"sb_upcoming": message, "upcoming_time": subtitle, "shows_index": index};
    }
    
    var transactionId = Pebble.sendAppMessage(json,
                                               function(e) {
                                                   //console.log("Successfully delivered message with transactionId="
                                                   //+ e.data.transactionId);
                                                   index = index + 1;
                                                   sendNextMessage(type, messages, subtitles);
                                               },
                                               function(e) {
                                                   console.log("Unable to deliver message with transactionId="
                                                       + e.data.transactionId + " Error is: " + e.error.message);
                                                   messages.unshift();
                                                   subtitles.unshift();
                                                   sendNextMessage(type, messages, subtitles);
                                               }
                                              );
}

function fetchShows(cmd) {
    var response;
    var req = new XMLHttpRequest();
    var allShows = new Array();
    var allStatus = new Array();
    
    if(options.showsPaused == "noPaused") {
        cmd = cmd + "&paused=0";
    }
    
    else if(options.showsPaused == "onlyPaused") {
        cmd = cmd + "&paused=1";
    }
    
    req.open('GET', BASE_URL + cmd, true);
    req.onload = function(e) {
        //console.log(req.readyState);
        //console.log(req.status);
        if (req.readyState == 4 && req.status == 200) {
            response = JSON.parse(req.responseText);
            
            if (response.result == "success") {
                for(var key in response.data) {
                    //console.log(response.data[key].show_name + " is " + response.data[key].status);
                    //allShows.push(response.data[key].tvdbid);
                    allShows.push(response.data[key].show_name);
                    allStatus.push(response.data[key].status);
                }
                sendNextMessage("shows", allShows, allStatus);
            }
        }
        else {
            console.log("Error: " + req.status.toString());
        }
    }
    req.send(null);
}

function fetchUpcomingType() {
    var typeCount = 0;
    
    if(options.today == "true") {
        typeCount = typeCount + 1;
        fetchUpcoming("today", typeCount);
    }
    
    if(options.soon == "true") {
        typeCount = typeCount + 1;
        fetchUpcoming("soon", typeCount);
    }
    
    if(options.later == "true") {
        typeCount = typeCount + 1;
        fetchUpcoming("later", typeCount);
    }
}

function fetchUpcoming(cmd, typeCount) {
    var response;
    var req = new XMLHttpRequest();
    var url;
    
    //console.log(BASE_URL + "future&type=" + cmd + "&paused=0");
    
    if(options.showUpPaused == "true") {
        url = "future&type=" + cmd + "&paused=1";
    }
    
    else {
        url = "future&type=" + cmd + "&paused=0";
    }
    
    req.open('GET', BASE_URL + url, true);
    req.onload = function(e) {
        //console.log(req.readyState);
        //console.log(req.status);
        if(req.readyState == 4 && req.status == 200) {
            response = JSON.parse(req.responseText);
            
            if (response.result == "success") {
                for(var type in response.data) {
                    //console.log(type);
                    //console.log(response.data[type][0].show_name);
                    for(var key in response.data[type]) {
                        upcoming.push(response.data[type][key].show_name);
                        var subtitle = "";
                        
                        if(options.showUpTime == "true") {
                            subtitle = subtitle + " " + response.data[type][key].airs.split(' ')[1];
                        }
                        
                        if(options.showUpDay == "true") {
                            subtitle = subtitle + " " + response.data[type][key].airs.split(' ')[0].substring(0,3);
                        }
                        
                        if(options.showUpDate == "true") {
                            subtitle = subtitle + " " + response.data[type][key].airdate.split('-')[1] + "/" + response.data[type][key].airdate.split('-')[2];
                        }
                        
                        time.push(subtitle);
                    }
                    upcomingTypeCount = upcomingTypeCount + 1;
                }
                if(typeCount == upcomingTypeCount) {
                    upcomingTypeCount = 0;
                    sendNextMessage("upcoming", upcoming, time);
                }
            }
        }
        else {
            console.log("Error: " + req.status.toString());
        }
    }
    req.send(null);
}

function backlogSearch(cmd) {
	var response;
	var req = new XMLHttpRequest();
	
	req.open('GET', BASE_URL + cmd, true);
    req.onload = function(e) {
        if (req.readyState == 4 && req.status == 200) {
            //console.log(req.responseText);
            response = JSON.parse(req.responseText);
            
            if(response.result == "success") {
                console.log("Successfully Forced Backlog!");
                Pebble.sendAppMessage({"backlog": "Searching Backlog"});
            }
        }
        else {
            console.log("Error: " + req.status.toString());
            Pebble.sendAppMessage({"backlog": "Couldn't Connect"});
        }
	}
    req.send(null);
}

function restartSB(cmd) {
    var response;
    var req = new XMLHttpRequest();
    
    req.open('GET', BASE_URL + cmd, true);
    req.onload = function(e) {
        if(req.readyState == 4 && req.status == 200) {
            //console.log(req.responseText);
            response = JSON.parse(req.responseText);
            
            if(response.result == "success" && response.message == "SickBeard is restarting...") {
                console.log("Successfully Restarting Sickbeard");
                Pebble.sendAppMessage({"restart": "Sickbeard is Restarting"});
            }
        }
        else {
            console.log("Error: " + req.status.toString());
            Pebble.sendAppMessage({"restart": "Couldn't Connect"});
        }
    }
    req.send(null);
}

function testSB(cmd) {
    var response;
    var req = new XMLHttpRequest();
    
    req.open('GET', BASE_URL + cmd, true);
    req.onload = function(e) {
        if(req.readyState && req.status == 200) {
            //console.log(req.responseText);
            response = JSON.parse(req.responseText);
            
            if(response.result == "success" && response.message == "Pong") {
                console.log("Sickbeard is Currently Running");
                Pebble.sendAppMessage({"ping": "Sickbeard is Running"});
            }
        }
        else {
            console.log("Error: " + req.status.toString());
            Pebble.sendAppMessage({"ping": "Couldn't Connect"});
        }
    }
    req.send(null);
}

function shutdownSB(cmd) {
    var response;
    var req = new XMLHttpRequest();
    
    req.open('GET', BASE_URL + cmd, true);
    req.onload = function(e) {
        if(req.readyState == 4 && req.status == 200) {
            //console.log(req.responseText);
            response = JSON.parse(req.responseText);
            
            if(response.result == "success" && response.message == "SickBeard is shutting down...") {
                console.log("Successfully Restarting Sickbeard");
                Pebble.sendAppMessage({"shutdown": "Sickbeard Shutting Down"});
            }
        }
        else {
            console.log("Error: " + req.status.toString());
            Pebble.sendAppMessage({"shutdown": "Couldn't Connect"});
        }
    }
    req.send(null);
}

Pebble.addEventListener("appmessage", function(e) {
    console.log("App Message Received");
    
    if(BASE_URL == "http://:/api//?cmd=" || BASE_URL == "" || BASE_URL == null) {
        Pebble.showSimpleNotificationOnPebble("Sickbeard", "Please Configure Sickbeard App");
    }
    
    else {
    
        if (e.payload.shows) {
            //console.log("Fetching Shows with cmd: " + e.payload.shows);
            fetchShows(e.payload.shows);
        }
        
        if (e.payload.upcoming) {
            //console.log("Fetching Upcoming");
            fetchUpcomingType();
        }
        
        if (e.payload.backlog) {
            //console.log("Forcing Backlog with cmd: " + e.payload.backlog);
            backlogSearch(e.payload.backlog);
        }
        
        if (e.payload.restart) {
            //console.log("Restarting Sickbeard with cmd: " + e.payload.restart);
            restartSB(e.payload.restart);
        }
        
        if (e.payload.ping) {
            //console.log("Testing Connection with cmd: " + e.payload.ping);
            testSB(e.payload.ping);
        }
        
        if (e.payload.shutdown) {
            //console.log("Shutting Down Sickbeard with cmd: " + e.payload.shutdown);
            shutdownSB(e.payload.shutdown);
        }
    }
});