//
//  =================================================================
//
//    13.02.14   <--  Date of Last Modification.
//                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  -----------------------------------------------------------------
//
//  **** Module  :  rvapi_global  <interface>
//       ~~~~~~~~~
//  **** Project :  HTML5-based presentation system
//       ~~~~~~~~~
//  **** Content :  Global definitions
//       ~~~~~~~~~
//
//  (C) E. Krissinel 2014
//
//  =================================================================
//


#ifndef RVAPI_GLOBAL_H
#define RVAPI_GLOBAL_H


// Document modes

/// \brief Silent mode produces no report
#define RVAPI_MODE_Silent  0x00100000
/// \brief Html mode produces full HTML5 report
#define RVAPI_MODE_Html    0x00000001
/// \brief Xmli2 mode produces XML output for CCP4i2
#define RVAPI_MODE_Xmli2   0x00000002


// Document layouts

#define RVAPI_LAYOUT_Plain       0x00000000
#define RVAPI_LAYOUT_Header      0x00000001
#define RVAPI_LAYOUT_Toolbar     0x00000002
#define RVAPI_LAYOUT_Tabs        0x00000004
#define RVAPI_LAYOUT_Full        0x00000007

/// \brief SlimHeader contains centered title string and right-aligned
///        'Exit' button; do not use with Toolbar layout
#define RVAPI_LAYOUT_SlimHeader  0x00000008


/// Text alignments

#define RVAPI_STYLE_Default   ""
#define RVAPI_STYLE_Left      "text-align:left;"
#define RVAPI_STYLE_Right     "text-align:right;"
#define RVAPI_STYLE_Center    "text-align:center;"


/// Text styles

#define RVAPI_STYLE_TEXT_Default ""
#define RVAPI_STYLE_TEXT_Serif   "font-family:\"Arial\";font-size:0.9em;font-weight:normal;font-style:normal;"
#define RVAPI_STYLE_TEXT_Fixed   "font-family:\"Courier\";text-decoration:none;font-weight:normal;font-style:normal;"
#define RVAPI_STYLE_TEXT_Normal  "font-style:normal;font-weight:normal;"


/// Table styles. They must be kept compatible with styles defined
/// in jsrview.css

#define RVAPI_CSS_TABLE_Blue   "table-blue"
#define RVAPI_CSS_TABLE_Plain  "table-plain"


/// Suffix IDs for hidden widgets

#define RVAPI_TABLE_DD_Suffix  "_table_dd"

/// Tree node open states

#define RVAPI_TREE_NODE_Auto       "auto"
#define RVAPI_TREE_NODE_Open       "open"
#define RVAPI_TREE_NODE_Closed     "closed"
#define RVAPI_TREE_NODE_AlwaysOpen "always_open"


/// Button click commands

#define RVAPI_BUTTON_Void         "{void}"
#define RVAPI_BUTTON_Export       "{export}"
#define RVAPI_BUTTON_Popup        "{popup}"
#define RVAPI_BUTTON_Function     "{function}"
#define RVAPI_BUTTON_Display      "{display}"


/// Checkbox commands

#define RVAPI_CHECKBOX_ShowLine   "{showline}"


/// Onchange actions

#define RVAPI_INPUT_SubmitOnChange "submitForm(this.form);"
#define RVAPI_INPUT_SubmitOnClick  "submitForm(this.form);"


/// Plot legends

#define RVAPI_LEGEND_LOC_Off        ""
#define RVAPI_LEGEND_LOC_N          "n"
#define RVAPI_LEGEND_LOC_E          "e"
#define RVAPI_LEGEND_LOC_S          "s"
#define RVAPI_LEGEND_LOC_W          "w"
#define RVAPI_LEGEND_LOC_NE         "ne"
#define RVAPI_LEGEND_LOC_NW         "nw"
#define RVAPI_LEGEND_LOC_SE         "se"
#define RVAPI_LEGEND_LOC_SW         "sw"
#define RVAPI_LEGEND_PLACE_Outside  "outsideGrid"
#define RVAPI_LEGEND_PLACE_Inside   ""


/// Line styles in Graphs

#define RVAPI_LINE_Off              "off"
#define RVAPI_LINE_Solid            "solid"
#define RVAPI_LINE_Dashed           "dashed"
#define RVAPI_LINE_Dotted           "dotted"
#define RVAPI_LINE_Center           "-."
#define RVAPI_LINE_Bars             "bars"

/// Line markers in Graphs

#define RVAPI_MARKER_Off            "off"
#define RVAPI_MARKER_Circle         "circle"
#define RVAPI_MARKER_Diamond        "diamond"
#define RVAPI_MARKER_Square         "square"
#define RVAPI_MARKER_filledCircle   "filledCircle"
#define RVAPI_MARKER_filledDiamond  "filledDiamond"
#define RVAPI_MARKER_filledSquare   "filledSquare"


/// Colors http://html-color-codes.info/color-names/

/// Red color names

#define RVAPI_COLOR_IndianRed             "#CD5C5C"
#define RVAPI_COLOR_LightCoral            "#F08080"
#define RVAPI_COLOR_Salmon                "#FA8072"
#define RVAPI_COLOR_DarkSalmon            "#E9967A"
#define RVAPI_COLOR_LightSalmon           "#FFA07A"
#define RVAPI_COLOR_Crimson               "#DC143C"
#define RVAPI_COLOR_Red                   "#FF0000"
#define RVAPI_COLOR_FireBrick             "#B22222"
#define RVAPI_COLOR_DarkRed               "#8B0000"

/// Pink color names

#define RVAPI_COLOR_Pink                  "#FFC0CB"
#define RVAPI_COLOR_LightPink             "#FFB6C1"
#define RVAPI_COLOR_HotPink               "#FF69B4"
#define RVAPI_COLOR_DeepPink              "#FF1493"
#define RVAPI_COLOR_MediumVioletRed       "#C71585"
#define RVAPI_COLOR_PaleVioletRed         "#DB7093"

/// Orange color names

#define RVAPI_COLOR_Coral                 "#FF7F50"
#define RVAPI_COLOR_Tomato                "#FF6347"
#define RVAPI_COLOR_OrangeRed             "#FF4500"
#define RVAPI_COLOR_DarkOrange            "#FF8C00"
#define RVAPI_COLOR_Orange                "#FFA500"

/// Yellow color names

#define RVAPI_COLOR_Gold                  "#FFD700"
#define RVAPI_COLOR_Yellow                "#FFFF00"
#define RVAPI_COLOR_LightYellow           "#FFFFE0"
#define RVAPI_COLOR_LemonChiffon          "#FFFACD"
#define RVAPI_COLOR_LightGoldenrodYellow  "#FAFAD2"
#define RVAPI_COLOR_PapayaWhip            "#FFEFD5"
#define RVAPI_COLOR_Moccasin              "#FFE4B5"
#define RVAPI_COLOR_PeachPuff             "#FFDAB9"
#define RVAPI_COLOR_PaleGoldenrod         "#EEE8AA"
#define RVAPI_COLOR_Khaki                 "#F0E68C"
#define RVAPI_COLOR_DarkKhaki             "#BDB76B"

/// Purple color names

#define RVAPI_COLOR_Lavender              "#E6E6FA"
#define RVAPI_COLOR_Thistle               "#D8BFD8"
#define RVAPI_COLOR_Plum                  "#DDA0DD"
#define RVAPI_COLOR_Violet                "#EE82EE"
#define RVAPI_COLOR_Orchid                "#DA70D6"
#define RVAPI_COLOR_Fuchsia               "#FF00FF"
#define RVAPI_COLOR_Magenta               "#FF00FF"
#define RVAPI_COLOR_MediumOrchid          "#BA55D3"
#define RVAPI_COLOR_MediumPurple          "#9370DB"
#define RVAPI_COLOR_Amethyst              "#9966CC"
#define RVAPI_COLOR_BlueViolet            "#8A2BE2"
#define RVAPI_COLOR_DarkViolet            "#9400D3"
#define RVAPI_COLOR_DarkOrchid            "#9932CC"
#define RVAPI_COLOR_DarkMagenta           "#8B008B"
#define RVAPI_COLOR_Purple                "#800080"
#define RVAPI_COLOR_Indigo                "#4B0082"
#define RVAPI_COLOR_SlateBlue             "#6A5ACD"
#define RVAPI_COLOR_DarkSlateBlue         "#483D8B"
#define RVAPI_COLOR_MediumSlateBlue       "#7B68EE"

/// Green color names

#define RVAPI_COLOR_GreenYellow           "#ADFF2F"
#define RVAPI_COLOR_Chartreuse            "#7FFF00"
#define RVAPI_COLOR_LawnGreen             "#7CFC00"
#define RVAPI_COLOR_Lime                  "#00FF00"
#define RVAPI_COLOR_LimeGreen             "#32CD32"
#define RVAPI_COLOR_PaleGreen             "#98FB98"
#define RVAPI_COLOR_LightGreen            "#90EE90"
#define RVAPI_COLOR_MediumSpringGreen     "#00FA9A"
#define RVAPI_COLOR_SpringGreen           "#00FF7F"
#define RVAPI_COLOR_MediumSeaGreen        "#3CB371"
#define RVAPI_COLOR_SeaGreen              "#2E8B57"
#define RVAPI_COLOR_ForestGreen           "#228B22"
#define RVAPI_COLOR_Green                 "#008000"
#define RVAPI_COLOR_DarkGreen             "#006400"
#define RVAPI_COLOR_YellowGreen           "#9ACD32"
#define RVAPI_COLOR_OliveDrab             "#6B8E23"
#define RVAPI_COLOR_Olive                 "#808000"
#define RVAPI_COLOR_DarkOliveGreen        "#556B2F"
#define RVAPI_COLOR_MediumAquamarine      "#66CDAA"
#define RVAPI_COLOR_DarkSeaGreen          "#8FBC8F"
#define RVAPI_COLOR_LightSeaGreen         "#20B2AA"
#define RVAPI_COLOR_DarkCyan              "#008B8B"
#define RVAPI_COLOR_Teal                  "#008080"

/// Blue color names

#define RVAPI_COLOR_Aqua                  "#00FFFF"
#define RVAPI_COLOR_Cyan                  "#00FFFF"
#define RVAPI_COLOR_LightCyan             "#E0FFFF"
#define RVAPI_COLOR_PaleTurquoise         "#AFEEEE"
#define RVAPI_COLOR_Aquamarine            "#7FFFD4"
#define RVAPI_COLOR_Turquoise             "#40E0D0"
#define RVAPI_COLOR_MediumTurquoise       "#48D1CC"
#define RVAPI_COLOR_DarkTurquoise         "#00CED1"
#define RVAPI_COLOR_CadetBlue             "#5F9EA0"
#define RVAPI_COLOR_SteelBlue             "#4682B4"
#define RVAPI_COLOR_LightSteelBlue        "#B0C4DE"
#define RVAPI_COLOR_PowderBlue            "#B0E0E6"
#define RVAPI_COLOR_LightBlue             "#ADD8E6"
#define RVAPI_COLOR_SkyBlue               "#87CEEB"
#define RVAPI_COLOR_LightSkyBlue          "#87CEFA"
#define RVAPI_COLOR_DeepSkyBlue           "#00BFFF"
#define RVAPI_COLOR_DodgerBlue            "#1E90FF"
#define RVAPI_COLOR_CornflowerBlue        "#6495ED"
#define RVAPI_COLOR_MediumSlateBlue       "#7B68EE"
#define RVAPI_COLOR_RoyalBlue             "#4169E1"
#define RVAPI_COLOR_Blue                  "#0000FF"
#define RVAPI_COLOR_MediumBlue            "#0000CD"
#define RVAPI_COLOR_DarkBlue              "#00008B"
#define RVAPI_COLOR_Navy                  "#000080"
#define RVAPI_COLOR_MidnightBlue          "#191970"

/// Brown color names

#define RVAPI_COLOR_Cornsilk              "#FFF8DC"
#define RVAPI_COLOR_BlanchedAlmond        "#FFEBCD"
#define RVAPI_COLOR_Bisque                "#FFE4C4"
#define RVAPI_COLOR_NavajoWhite           "#FFDEAD"
#define RVAPI_COLOR_Wheat                 "#F5DEB3"
#define RVAPI_COLOR_BurlyWood             "#DEB887"
#define RVAPI_COLOR_Tan                   "#D2B48C"
#define RVAPI_COLOR_RosyBrown             "#BC8F8F"
#define RVAPI_COLOR_SandyBrown            "#F4A460"
#define RVAPI_COLOR_Goldenrod             "#DAA520"
#define RVAPI_COLOR_DarkGoldenrod         "#B8860B"
#define RVAPI_COLOR_Peru                  "#CD853F"
#define RVAPI_COLOR_Chocolate             "#D2691E"
#define RVAPI_COLOR_SaddleBrown           "#8B4513"
#define RVAPI_COLOR_Sienna                "#A0522D"
#define RVAPI_COLOR_Brown                 "#A52A2A"
#define RVAPI_COLOR_Maroon                "#800000"

/// White color names

#define RVAPI_COLOR_White                 "#FFFFFF"
#define RVAPI_COLOR_Snow                  "#FFFAFA"
#define RVAPI_COLOR_Honeydew              "#F0FFF0"
#define RVAPI_COLOR_MintCream             "#F5FFFA"
#define RVAPI_COLOR_Azure                 "#F0FFFF"
#define RVAPI_COLOR_AliceBlue             "#F0F8FF"
#define RVAPI_COLOR_GhostWhite            "#F8F8FF"
#define RVAPI_COLOR_WhiteSmoke            "#F5F5F5"
#define RVAPI_COLOR_Seashell              "#FFF5EE"
#define RVAPI_COLOR_Beige                 "#F5F5DC"
#define RVAPI_COLOR_OldLace               "#FDF5E6"
#define RVAPI_COLOR_FloralWhite           "#FFFAF0"
#define RVAPI_COLOR_Ivory                 "#FFFFF0"
#define RVAPI_COLOR_AntiqueWhite          "#FAEBD7"
#define RVAPI_COLOR_Linen                 "#FAF0E6"
#define RVAPI_COLOR_LavenderBlush         "#FFF0F5"
#define RVAPI_COLOR_MistyRose             "#FFE4E1"

/// Grey color names

#define RVAPI_COLOR_Gainsboro             "#DCDCDC"
#define RVAPI_COLOR_LightGrey             "#D3D3D3"
#define RVAPI_COLOR_Silver                "#C0C0C0"
#define RVAPI_COLOR_DarkGray              "#A9A9A9"
#define RVAPI_COLOR_Gray                  "#808080"
#define RVAPI_COLOR_DimGray               "#696969"
#define RVAPI_COLOR_LightSlateGray        "#778899"
#define RVAPI_COLOR_SlateGray             "#708090"
#define RVAPI_COLOR_DarkSlateGray         "#2F4F4F"
#define RVAPI_COLOR_Black                 "#000000"


#endif // RVAPI_GLOBAL_H
