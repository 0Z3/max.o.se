{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 2,
			"revision" : 1,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 526.0, 134.0, 983.0, 594.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-14",
					"linecount" : 7,
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 127.0, 362.0, 266.0, 104.0 ],
					"text" : "{\n\r /foo : [ 1, 2, 3 ],\n\r /bar : [ 4, 5, 6 ]\n\r}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-13",
					"linecount" : 7,
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 127.0, 241.0, 266.0, 104.0 ],
					"text" : "{\n\r /foo : [ 1, 2, 3 ],\n\r /bar : [ 4, 5, 6 ]\n\r}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"linecount" : 4,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 1,
					"o.se_initfile" : "o.compose.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 127.0, 152.0, 146.0, 64.0 ],
					"text" : "{\n/foo : [1, 2, 3],\n/bar : [4, 5, 6]\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 127.0, 96.0, 42.0, 22.0 ],
					"text" : "debug"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-14", 0 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "o.se.gui.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
