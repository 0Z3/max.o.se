{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 2,
			"revision" : 2,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 422.0, 126.0, 1244.0, 799.0 ],
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
					"id" : "obj-16",
					"linecount" : 3,
					"maxclass" : "o.display",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 745.0, 212.0, 150.0, 59.0 ],
					"text" : "/x : 33,\n/y : 43,\n/z : 1419"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"id" : "obj-15",
					"maxclass" : "o.display",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 979.0, 439.0, 150.0, 33.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-10",
					"linecount" : 3,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 1,
					"o.se_initfile" : "o.compose.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 1007.0, 364.5, 146.0, 51.0 ],
					"text" : "{\n/x : 22\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-12",
					"linecount" : 3,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 1,
					"o.se_initfile" : "o.compose.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 950.0, 34.0, 146.0, 51.0 ],
					"text" : "{\n/x : 22\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-11",
					"linecount" : 5,
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 903.0, 212.0, 266.0, 89.0 ],
					"text" : "{\n /x : 33,\n /y : 43,\n /z : 1419\n}",
					"textcolor" : [ 1.0, 1.0, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"linecount" : 3,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 2,
					"o.se_initfile" : "o.expr.codebox.ose",
					"outlettype" : [ "FullPacket", "FullPacket" ],
					"patching_rect" : [ 903.0, 124.0, 141.0, 51.0 ],
					"text" : "/x = 33,\n/y = /x + 10,\n/z = /x * /y",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-8",
					"linecount" : 4,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 2,
					"o.se_initfile" : "o.expr.codebox.ose",
					"outlettype" : [ "FullPacket", "FullPacket" ],
					"patching_rect" : [ 227.0, 591.0, 84.0, 91.0 ],
					"text" : "{\n/foo : [1, 2, 3],\n/bar : [4, 6, 7]\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"id" : "obj-7",
					"maxclass" : "o.compose",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 727.0, 398.0, 150.0, 23.0 ],
					"saved_bundle_data" : [ 35, 98, 117, 110, 100, 108, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 47, 102, 111, 111, 0, 0, 0, 0, 44, 105, 0, 0, 0, 0, 0, 10 ],
					"saved_bundle_length" : 36,
					"text" : "/foo : 10"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-6",
					"linecount" : 3,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 1,
					"o.se_initfile" : "o.compose.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 600.0, 472.0, 146.0, 51.0 ],
					"text" : "{\n /foo : 10\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-5",
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 435.0, 216.0, 266.0, 24.0 ],
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4",
					"linecount" : 4,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 1,
					"o.se_initfile" : "o.compose.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 435.0, 49.0, 146.0, 64.0 ],
					"text" : "{\n/foo : [1, 2, 3],\n/bar : [4, 5, 6]\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-2",
					"linecount" : 3,
					"maxclass" : "o.se.gui",
					"numinlets" : 2,
					"numoutlets" : 2,
					"o.se_initfile" : "o.expr.codebox.ose",
					"outlettype" : [ "FullPacket", "FullPacket" ],
					"patching_rect" : [ 435.0, 147.0, 300.0, 51.0 ],
					"text" : "{\n/@/bloo : [\"/$/foo\", \"/$/bar\", \"/!/push\"]\n}",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-14",
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 127.0, 378.0, 266.0, 24.0 ],
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-13",
					"maxclass" : "o.se.gui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"o.se_initfile" : "o.display.ose",
					"outlettype" : [ "FullPacket" ],
					"patching_rect" : [ 127.0, 241.0, 266.0, 24.0 ],
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
					"destination" : [ "obj-15", 0 ],
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-9", 0 ],
					"source" : [ "obj-12", 0 ]
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
					"destination" : [ "obj-5", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-6", 1 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-11", 0 ],
					"order" : 0,
					"source" : [ "obj-9", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-16", 0 ],
					"order" : 1,
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "o.compose.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "o.display.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "o.se.gui.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
