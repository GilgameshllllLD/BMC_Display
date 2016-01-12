(function($){
$.extend({
	websocketSettings: {
		open: function(){},
		close: function(){},
		message: function(){},
		options: {},
		events: {}
	},
	websocket: function(url, s) {
		var ws = WebSocket ? new WebSocket( url ) : {
			send: function(m){ return false },
			close: function(){}
		};
		$.websocketSettings = $.extend($.websocketSettings, s);
		$(ws)
			.bind('open', $.websocketSettings.open)
			.bind('close', $.websocketSettings.close)
			.bind('message', $.websocketSettings.message)
			.bind('message', function(e){
				var m = jQuery.parseJSON(e.originalEvent.data);
				var h = $.websocketSettings.events[m.event];
				if (h) h.call(this, m);
			});
		ws._settings = $.extend($.websocketSettings, s);
		ws._send = ws.send;
		ws.send = function(data) {
			return this._send(data);
		}
		$(window).unload(function(){ ws.close(); ws = null });
		return ws;
	}
});
})(jQuery);

var ws;
var chart1,chart2;
var timeresourcedata = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var timequerydata = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var timeappdata = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var timesysdata = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
var dayresourcedata = [0,0,0,0,0,0,0];
var dayquerydata = [0,0,0,0,0,0,0];
var dayappdata = [0,0,0,0,0,0,0];
var daysysdata = [0,0,0,0,0,0,0];
var fileid = '1';

var MessageDataSets = [new TimeSeries(), new TimeSeries()];

  $(document).ready(function () {
    $('#modalConnected').modal({'show' : true, 'backdrop' : 'static'});
    // init tooltips
    $("[rel=tooltip]").tooltip({'placement': 'right'});
    
    var options = [
      { strokeStyle: 'rgba(174, 67, 46, 1)', fillStyle: 'rgba(174, 67, 46, 0.3)', lineWidth: 1.25 },
      { strokeStyle: 'rgba(119, 171, 19, 1)', fillStyle: 'rgba(119, 171, 19, 0.1)', lineWidth: 1.25 }
    ];
        
    var monitor = new SmoothieChart({ minValue: -1, maxValueScale: 1.25, interpolation: 'bezier',  millisPerPixel: 20, grid: { fillStyle: '#222', strokeStyle: '#fff', lineWidth: 0.05, millisPerLine: 1000, verticalSections: 4 }});
    for (var i = 0; i < MessageDataSets.length; i++) {
        monitor.addTimeSeries(MessageDataSets[i], options[i]);
    }
    monitor.streamTo(document.getElementById('elapsedcanvas'), 1000);
    
    Highcharts.setOptions({
	colors: ["#79f", "#de7c18", "#55BF3B", "#DF5353"],
	chart: {
		backgroundColor: {
			linearGradient: [0, 0, 0, 400],
			stops: [
				[0, 'rgb(96, 96, 96)'],
				[1, 'rgb(16, 16, 16)']
			]
		},
		borderWidth: 0,
		borderRadius: 15,
		plotBackgroundColor: null,
		plotShadow: false,
		plotBorderWidth: 0
	},
	title: {
		style: { 
			color: '#FFF',
			font: '16px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
		}
	},
	subtitle: {
		style: { 
			color: '#DDD',
			font: '12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
		}
	},
	xAxis: {
		gridLineWidth: 0,
		lineColor: '#999',
		tickColor: '#999',
		labels: {
			style: {
				color: '#999',
				fontWeight: 'bold'
			}
		},
		title: {
			style: {
				color: '#AAA',
				font: 'bold 12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
			}				
		}
	},
	yAxis: {
		alternateGridColor: null,
		minorTickInterval: null,
		gridLineColor: 'rgba(255, 255, 255, .1)',
		lineWidth: 0,
		tickWidth: 0,
		labels: {
			style: {
				color: '#999',
				fontWeight: 'bold'
			}
		},
		title: {
			style: {
				color: '#AAA',
				font: 'bold 12px Lucida Grande, Lucida Sans Unicode, Verdana, Arial, Helvetica, sans-serif'
			}				
		}
	},
	legend: {
		itemStyle: {
			color: '#CCC'
		},
		itemHoverStyle: {
			color: '#FFF'
		},
		itemHiddenStyle: {
			color: '#333'
		}
	},
	labels: {
		style: {
			color: '#CCC'
		}
	},
	tooltip: {
		backgroundColor: {
			linearGradient: [0, 0, 0, 50],
			stops: [
				[0, 'rgba(96, 96, 96, .8)'],
				[1, 'rgba(16, 16, 16, .8)']
			]
		},
		borderWidth: 0,
		style: {
			color: '#FFF'
		}
	},
	
	
	plotOptions: {
		line: {
			dataLabels: {
				color: '#CCC'
			},
			marker: {
				lineColor: '#333'
			}
		},
		spline: {
			marker: {
				lineColor: '#333'
			}
		},
		scatter: {
			marker: {
				lineColor: '#333'
			}
		}
	},
	
	toolbar: {
		itemStyle: {
			color: '#CCC'
		}
	},
	
	navigation: {
		buttonOptions: {
			backgroundColor: {
				linearGradient: [0, 0, 0, 20],
				stops: [
					[0.4, '#606060'],
					[0.6, '#333333']
				]
			},
			borderColor: '#000000',
			symbolStroke: '#C0C0C0',
			hoverSymbolStroke: '#FFFFFF'
		}
	},
	
	exporting: {
		buttons: {
			exportButton: {
				symbolFill: '#55BE3B'
			},
			printButton: {
				symbolFill: '#7797BE'
			}
		}
	},	
	
	// special colors for some of the
	legendBackgroundColor: 'rgba(48, 48, 48, 0.8)',
	legendBackgroundColorSolid: 'rgb(70, 70, 70)',
	dataLabelsColor: '#444',
	maskColor: 'rgba(255,255,255,0.3)'}
    );// end highcharts setup
    
    chart1 = new Highcharts.Chart({
      chart: {
         renderTo: 'chart1',
         defaultSeriesType: 'spline',
         marginRight: 130,
         marginBottom: 50
      },
      title: {
         text: 'Node Operations',
         x: -20 //center
      },
      subtitle: {
         text: 'by Time',
         x: -20
      },
      xAxis: {
		labels: {
            rotation: -45,
            y: 30
		},
         plotBands: [{
            from: 24,
            to: 24.1,
            color: 'rgba(68, 170, 213, 0.6)'
         },{
            from: 16.5,
            to: 34.5,
            color: 'rgba(0, 0, 0, 0.1)'
         }],
        categories: [' ', ' ', ' ', ' ', '2:00', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '6:00', ' ', ' ', ' ', '8:00', ' ', ' ', ' ', '10:00', ' ', ' ', ' ', 'Noon', ' ', ' ', ' ', '2:00', ' ', ' ', ' ', '4:00', ' ', ' ', ' ', '6:00', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '10:00', ' ', ' ', ' ']
      },
      yAxis: {
         title: {
            text: 'Counter'
         },
         plotLines: [{
            value: 0,
            width: 1,
            color: '#808080'
         }]
      },
      tooltip: {
         enabled: false
      },
      plotOptions: {
         spline: {
            lineWidth: 1,
            states: {
               hover: {
                  lineWidth: 1.5
               }
            },
            marker: {
               enabled: false,
               states: {
                  hover: {
                     enabled: false,
                     symbol: 'circle',
                     radius: 5,
                     lineWidth: 1
                  }
               }   
            }//,
            //pointInterval: 3600000, // one hour
            //pointStart: Date.UTC(2009, 9, 6, 0, 0, 0)
         }
      },
      legend: {
         layout: 'vertical',
         align: 'right',
         verticalAlign: 'top',
         x: -10,
         y: 100,
         borderWidth: 0
      },
      series: [
      //{
      //   name: 'Messages',
      //   data: timemessagedata
      //},
      {  name: 'Resources',
         data: timeresourcedata
      },
      {
	     name: 'Queries',
	     data: timequerydata
      },
      {
	     name: 'App',
	     data: timeappdata
      },
      {
	     name: 'System',
	     data: timesysdata
      }
      ]
    });
    
    chart2 = new Highcharts.Chart({
      chart: {
         renderTo: 'chart2',
         defaultSeriesType: 'spline',
         marginRight: 130,
         marginBottom: 25
      },
      title: {
         text: 'Node Operations',
         x: -20 //center
      },
      subtitle: {
         text: 'by Day',
         x: -20
      },
      xAxis: {
         categories: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'],
         plotBands: [{
            from: 4.5,
            to: 7,
            color: 'rgba(0, 0, 0, 0.3)'
         }]
      },
      yAxis: {
         title: {
            text: 'Counter'
         },
         plotLines: [{
            value: 0,
            width: 1,
            color: '#808080'
         }]
      },
      tooltip: {
         enabled: false
      },
      plotOptions: {
         spline: {
            lineWidth: 1,
            states: {
               hover: {
                  lineWidth: 1.5
               }
            },
            marker: {
               enabled: false  
            },
            //pointInterval: 3600000, // one hour
            //pointStart: Date.UTC(2009, 9, 6, 0, 0, 0)
         }
      },
      legend: {
         layout: 'vertical',
         align: 'right',
         verticalAlign: 'top',
         x: -10,
         y: 100,
         borderWidth: 0
      },
      series: [
      //{
      //   name: 'Messages',
      //   data: timemessagedata
      //},
      {  name: 'Resources',
         data: dayresourcedata
      },
      {
	     name: 'Queries',
	     data: dayquerydata
      },
      {
	     name: 'App',
	     data: dayappdata
      },
      {
	     name: 'System',
	     data: daysysdata
      }
      ]
    });
    
    ws = $.websocket("ws://127.0.0.1:9000/", {
            open: function() {
                $('#modalConnected').modal('hide');
                $('span#connect-status').removeClass('label-important');
                $('span#connect-status').addClass('label-success');
                $('span#connect-status').html('ONLINE');            
            },
            close: function() {
                $('#modalConnected').modal('show');
                $('span#connect-status').removeClass('label-success');
                $('span#connect-status').addClass('label-important');
                $('span#connect-status').html('OFFLINE');
            },
            events: {
                    stats: function(e) {
                    	chart1.series[0].setData(e.eventdata.resources);
                    	chart1.series[1].setData(e.eventdata.queries);
                    	chart1.series[2].setData(e.eventdata.app);
                    	chart1.series[3].setData(e.eventdata.system);
                    	chart1.redraw();
                    },
                    daystats: function(e) {
                    	chart2.series[0].setData(e.eventdata.resources);
                    	chart2.series[1].setData(e.eventdata.queries);
                    	chart2.series[2].setData(e.eventdata.app);
                    	chart2.series[3].setData(e.eventdata.system);
                    	chart2.redraw();
                    },
                    log: function(e) {
                        // append row to table
                        $('<tr><td>' + e.eventdata.level.Name + '</td><td>' + e.eventdata.timestamp + '</td><td>' + e.eventdata.message + '</td></tr>').prependTo('#logger > tbody');
                        // pop of the last table row if there are more than 150 rows
                        var rowCount = $('#logger tr').length;
                        if(rowCount > 150) {
                            $('#logger tr:last').remove();
                        }
                    },
                    // this is the data from rmq api
                    queue: function(e) {
                        //console.log(e.eventdata);
                        var now = new Date().getTime();
                        $('td#messages').html(e.eventdata.messages);
                        MessageDataSets[0].append(now, e.eventdata.messages);
                        MessageDataSets[1].append(now, e.eventdata.messages_ready);
                        //MessageDataSets[2].append(now, e.eventdata.messages_unacknowledged);
                    },
                    init: function(e) {
                        // uptime
                        $('#uptime').attr('datetime', e.eventdata.starttime);
                        $('#uptime').relative();
                        // rebuild time (if it exists)
                        if(e.eventdata.rebuildtime.length > 0) {
                            $('<time id="rebuildtime" datetime=""></time>').appendTo('#rebuildholder');
                            $('#rebuildtime').attr('datetime', e.eventdata.rebuildtime);
                            $('#rebuildtime').relative({format:"human",displayZeros:false});
                        }
                        
                        // profile
                        $('#profile').html('Profile: ' + e.eventdata.profile);
                        
                        // messages processed
                        $('td#messages_ack').html(e.eventdata.messages_ack);
                        $('td#messages_nack').html(e.eventdata.messages_nack);
                        $('td#sysmessages').html(e.eventdata.sysmessages);
                        $('td#dbmessages').html(e.eventdata.dbmessages);
                        $('td#appmessages').html(e.eventdata.appmessages);
                        $('td#resourcemessages').html(e.eventdata.resourcemessages);
                    },
                    messages_ack: function(e) {
                        $('td#messages_ack').html(e.eventdata.counter);
                    },
                    messages_nack: function(e) {
                        $('td#messages_nack').html(e.eventdata.counter);
                    },
                    sysmessages: function(e) {
                        $('td#sysmessages').html(e.eventdata.counter).effect("highlight", {}, 500);
                    },
                    appmessages: function(e) {
                        $('td#appmessages').html(e.eventdata.counter).effect("highlight", {}, 500);
                    },
                    dbmessages: function(e) {
                        $('td#dbmessages').html(e.eventdata.counter).effect("highlight", {}, 500);
                    },
                    resourcemessages: function(e) {
                        $('td#resourcemessages').html(e.eventdata.counter).effect("highlight", {}, 500);
                    },
                    rebuildtime: function(e) {
                        $('#rebuildtime').remove();
                        $('<time id="rebuildtime" datetime=""></time>').appendTo('#rebuildholder');
                        $('#rebuildtime').attr('datetime', e.eventdata.time);
                        $('#rebuildtime').relative({format:"human",displayZeros:false});
                    },
                    status: function(e) {
                        $('span#status').html(e.eventdata.status);
                        if(e.eventdata.status == 'Idle') {
                            $('span#status').removeClass('label-important');
                            $('span#status').addClass('label-info');    
                        } else {
                            $('span#status').removeClass('label-info');
                            $('span#status').addClass('label-important');                        
                        }
                    },
                    progress: function(e) {
                        var resource = $('tr#resource_' + fileid);
                        if(resource.length == 1) {
                            $('span.download').html(e.eventdata.progress + '%');
                            if(e.eventdata.progress == 100) {
                                $('span.download').removeClass('label-important');
                                $('span.download').addClass('label-success');
                                $('span.download').removeClass('download'); 
                                fileid++;
                            }
                        } else {
                            // append row to table
                            $('<tr id="resource_' + fileid + '"><td>' + e.eventdata.file + '</td><td>' + e.eventdata.filesize + '</td><td><span class="download label label-important">' + e.eventdata.progress + '%</span></td></tr>').prependTo('#resources > tbody');
                            // pop of the last table row if there are more than 150 rows
                            var rowCount = $('#resources tr').length;
                            if(rowCount > 150) {
                                $('#resources tr:last').remove();
                            }
                        }
                    }
            }
    });
    
  });
  
function bugreport() {
    var desc = $('#frmBugDesc').val();
    var detail = $('#frmBugDetail').val();
    var msg = {};
    msg.method = 'bugreport';
    msg.data = [];
    msg.data[0] = desc;
    msg.data[1] = detail;
    ws.send(JSON.stringify(msg));
    $('#frmBugDesc').val('');
    $('#frmBugDetail').val('');
}

function showTab(nav, tab) {
    $('li.tab').each(function(index) {
        $(this).removeClass('active');    
    });
    $(nav).parent().addClass('active');
    $('div.tabpane').each(function(index) {
        $(this).addClass('tabpane-hidden');    
    });
    $('#'+tab).removeClass('tabpane-hidden');
}