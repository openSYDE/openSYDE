//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header file with constants

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QColor>
#include <QFont>
#include <QSize>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */
//Modes and Submodes
const int32_t ms32_MODE_NONE = -1;
const int32_t ms32_MODE_SYSDEF = 0;
const int32_t ms32_MODE_SYSVIEW = 1;
const int32_t ms32_SUBMODE_SYSDEF_TOPOLOGY = 0;
const int32_t ms32_SUBMODE_SYSDEF_NODEEDIT = 1;
const int32_t ms32_SUBMODE_SYSDEF_BUSEDIT = 2;
const int32_t ms32_SUBMODE_SYSVIEW_SETUP = 0;
const int32_t ms32_SUBMODE_SYSVIEW_UPDATE = 1;
const int32_t ms32_SUBMODE_SYSVIEW_DASHBOARD = 2;
//Flags for specific functions of modes
const uint32_t mu32_FLAG_DEFAULT = 0U;
const uint32_t mu32_FLAG_EDIT_NAME = 1U;
const uint32_t mu32_FLAG_OPEN_PROPERTIES = 2U;
//Flags for specific functions of bus modes
const uint32_t mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR = 3U;
//The COMM messages flag gets the protocol type as offset.
const uint32_t mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR_PROTOCOL = 0x80000000U;
//Flags for specific functions of node modes
//The CANopen Manager flag gets the interface number as offset.
const uint32_t mu32_FLAG_OPEN_SYSDEF_CANOPENMANAGER = 0x40000000U;
//The CANopen Manager flag gets the device as offset.
const uint32_t mu32_FLAG_OPEN_SYSDEF_DEVICENODE_IN_CANOPENMANAGER = 0x80000000U;

//CANopen Manager
const float32_t mf32_HEARTBEAT_CONSUMER_TIME_FACTOR = 1.5F;

//Animation timer
const int32_t ms32_ANIMATION_STEPS = 10;
const int32_t ms32_TIMER_INTERVAL = 10; //ms

//GUI refresh timer
const int32_t ms32_TIMER_GUI_REFRESH = 100; //ms

//Dashboard timeout factor
const uint32_t mu32_DASHBOARD_TIMEOUT_FACTOR = 10U;
const uint32_t mu32_DASHBOARD_TIMEOUT_TOLERANCE = 20; // Tolerance in percentage

//Interaction point size
const float64_t mf64_INTERACTION_POINT_WIDTH = 12.0;

//Colors
const QColor mc_HOVER_SHADOW_COLOR = QColor(180, 180, 180, 180);

//Style guide colors (design studio)
const QColor mc_STYLE_GUIDE_COLOR_0 = QColor(255, 255, 255, 255);
const QColor mc_STYLE_GUIDE_COLOR_1 = QColor(19, 19, 45, 255);
const QColor mc_STYLE_GUIDE_COLOR_2 = QColor(27, 27, 68, 255);
const QColor mc_STYLE_GUIDE_COLOR_2_50P = QColor(141, 141, 161, 255);
const QColor mc_STYLE_GUIDE_COLOR_3 = QColor(42, 42, 91, 255);
const QColor mc_STYLE_GUIDE_COLOR_4 = QColor(57, 57, 109, 255);
const QColor mc_STYLE_GUIDE_COLOR_5 = QColor(71, 71, 99, 255);
const QColor mc_STYLE_GUIDE_COLOR_6 = QColor(86, 86, 104, 255);
const QColor mc_STYLE_GUIDE_COLOR_7 = QColor(87, 87, 170, 255);
const QColor mc_STYLE_GUIDE_COLOR_8 = QColor(130, 130, 143, 255);
const QColor mc_STYLE_GUIDE_COLOR_9 = QColor(155, 155, 170, 255);
const QColor mc_STYLE_GUIDE_COLOR_10 = QColor(206, 206, 217, 255);
const QColor mc_STYLE_GUIDE_COLOR_11 = QColor(237, 237, 242, 255);
const QColor mc_STYLE_GUIDE_COLOR_12 = QColor(250, 250, 250, 255);
const QColor mc_STYLE_GUIDE_COLOR_13 = QColor(126, 199, 98, 255);
const QColor mc_STYLE_GUIDE_COLOR_14 = QColor(169, 143, 247, 255);
const QColor mc_STYLE_GUIDE_COLOR_15 = QColor(114, 247, 247, 255);
const QColor mc_STYLE_GUIDE_COLOR_16 = QColor(19, 128, 110, 255);
const QColor mc_STYLE_GUIDE_COLOR_17 = QColor(247, 147, 30, 255);
const QColor mc_STYLE_GUIDE_COLOR_18 = QColor(255, 0, 0, 255);
const QColor mc_STYLE_GUIDE_COLOR_21 = QColor(94, 163, 67, 255);
const QColor mc_STYLE_GUIDE_COLOR_22 = QColor(203, 235, 193, 255);
const QColor mc_STYLE_GUIDE_COLOR_23 = QColor(170, 54, 54, 255);
const QColor mc_STYLE_GUIDE_COLOR_24 = QColor(230, 58, 63, 255);
const QColor mc_STYLE_GUIDE_COLOR_25 = QColor(195, 186, 221, 255);
const QColor mc_STYLE_GUIDE_COLOR_26 = QColor(2, 155, 136, 255);
const QColor mc_STYLE_GUIDE_COLOR_26_40P = QColor(154, 215, 207, 255);
const QColor mc_STYLE_GUIDE_COLOR_27 = QColor(127, 127, 187, 255);
const QColor mc_STYLE_GUIDE_COLOR_31 = QColor(20, 20, 20, 255);
const QColor mc_STYLE_GUIDE_COLOR_32 = QColor(30, 30, 33, 255);
const QColor mc_STYLE_GUIDE_COLOR_33 = QColor(47, 48, 54, 255);
const QColor mc_STYLE_GUIDE_COLOR_34 = QColor(91, 91, 91, 255);
const QColor mc_STYLE_GUIDE_COLOR_35 = QColor(156, 155, 155, 255);
const QColor mc_STYLE_GUIDE_COLOR_36 = QColor(177, 177, 177, 255);
const QColor mc_STYLE_GUIDE_COLOR_37 = QColor(192, 192, 192, 255);
const QColor mc_STYLE_GUIDE_COLOR_38 = QColor(217, 217, 226, 255);
const QColor mc_STYLE_GUIDE_COLOR_39 = QColor(116, 116, 116, 255);
const QColor mc_STYLE_GUIDE_COLOR_40 = QColor(97, 97, 150, 255);
const QColor mc_STYLE_GUIDE_COLOR_52 = QColor(66, 66, 66, 255);
const QColor mc_STYLE_GUIDE_COLOR_67 = QColor(32, 31, 67, 255);
const QColor mc_STYLE_GUIDE_COLOR_1353 = QColor(22, 22, 44, 255);
const QColor mc_STYLE_GUIDE_COLOR_2706 = QColor(83, 83, 117);
const QColor mc_STYLE_GUIDE_COLOR_3001 = QColor(73, 73, 133);
const QColor mc_STYLE_GUIDE_COLOR_5413 = QColor(126, 126, 188);
const QColor mc_STYLE_GUIDE_COLOR_LINK = mc_STYLE_GUIDE_COLOR_4;

const QColor mc_STYLE_GUIDE_COLOR_CH1 = QColor(126, 199, 98);
const QColor mc_STYLE_GUIDE_COLOR_CH2 = QColor(90, 90, 201);
const QColor mc_STYLE_GUIDE_COLOR_CH3 = QColor(92, 92, 92);
const QColor mc_STYLE_GUIDE_COLOR_CH4 = QColor(140, 140, 141);

//Style guide colors (design studio) for dynamic stylesheets
const QString mc_STYLESHEET_GUIDE_COLOR_0 = "rgb(255, 255, 255)";
const QString mc_STYLESHEET_GUIDE_COLOR_4 = "rgb(57, 57, 109)";
const QString mc_STYLESHEET_GUIDE_COLOR_8 = "rgb(130, 130, 143)";
const QString mc_STYLESHEET_GUIDE_COLOR_10 = "rgb(206, 206, 217)";
const QString mc_STYLESHEET_GUIDE_COLOR_11 = "rgb(237, 237, 242)";
const QString mc_STYLESHEET_GUIDE_COLOR_12 = "rgb(250, 250, 250)";
const QString mc_STYLESHEET_GUIDE_COLOR_24 = "rgb(230, 58, 63)";
const QString mc_STYLESHEET_GUIDE_COLOR_LINK = "rgb(57, 57, 109)";

//Style guide fonts (design studio)
const QFont mc_STYLE_GUIDE_FONT_REGULAR_7  = QFont("Segoe UI", 7);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_10  = QFont("Segoe UI", 10);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_11  = QFont("Segoe UI", 11);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_12  = QFont("Segoe UI", 12);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_13  = QFont("Segoe UI", 13);
const QFont mc_STYLE_GUIDE_FONT_SEMIBOLD_13 = QFont("Segoe UI Semibold", 13);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_14  = QFont("Segoe UI", 14);
const QFont mc_STYLE_GUIDE_FONT_SEMIBOLD_14 = QFont("Segoe UI Semibold", 14);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_15  = QFont("Segoe UI", 15);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_16  = QFont("Segoe UI", 16);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_16_ITALIC  = QFont("Segoe UI", 16, -1, true);
const QFont mc_STYLE_GUIDE_FONT_SEMIBOLD_16 = QFont("Segoe UI Semibold", 16);
const QFont mc_STYLE_GUIDE_FONT_BOLD_16 = QFont("Segoe UI", 16, static_cast<int32_t>(QFont::Bold));
const QFont mc_STYLE_GUIDE_FONT_REGULAR_18  = QFont("Segoe UI", 18);
const QFont mc_STYLE_GUIDE_FONT_BOLD_18 = QFont("Segoe UI", 18, static_cast<int32_t>(QFont::Bold));
const QFont mc_STYLE_GUIDE_FONT_REGULAR_20  = QFont("Segoe UI", 20);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_23  = QFont("Segoe UI", 23);
const QFont mc_STYLE_GUIDE_FONT_SEMIBOLD_24 = QFont("Segoe UI Semibold", 24);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_25  = QFont("Segoe UI", 25);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_28  = QFont("Segoe UI", 28);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_30  = QFont("Segoe UI", 30);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_33  = QFont("Segoe UI", 33);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_35  = QFont("Segoe UI", 35);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_38  = QFont("Segoe UI", 38);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_40  = QFont("Segoe UI", 40);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_43  = QFont("Segoe UI", 43);
const QFont mc_STYLE_GUIDE_FONT_REGULAR_48  = QFont("Segoe UI", 48);

//Double spin boxes
const int32_t ms32_DOUBLE_SPIN_BOX_DECIMAL_COUNT = 300;

//Transparency configuration for timeout visualization
const int32_t ms32_TRANSPARENCY_START = 80;
const int32_t ms32_TRANSPARENCY_END = 255;

//Name restrictions
const int32_t ms32_C_ITEM_MAX_CHAR_COUNT = 31;

//Node id restrictions
const uint8_t mu8_NODE_ID_CLIENT = 126; //127 is reserved for broadcasts
const uint8_t mu8_MIN_NODE_ID_OS = 0;
const uint8_t mu8_MAX_NODE_ID_OS = 125;
const uint8_t mu8_MAX_NODE_ID_STW = static_cast<uint8_t> (254); //255 is reserved
const uint8_t mu8_MIN_NODE_ID_CANOPEN = static_cast<uint8_t> (1);
const uint8_t mu8_MAX_NODE_ID_CANOPEN = static_cast<uint8_t> (127);

//Data pool sizes
const uint32_t mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN = 0x101U;
const uint32_t mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX = 0x17FU;
const uint32_t mu32_PROTOCOL_ECES_SIGNALCOUNT_MAX = 48U;
const uint16_t mu16_SIGNAL_BIT_MAX = 64U;
//Data set restrictions
const uint32_t mu32_NODE_DATA_SET_PER_LIST_MAX = 16U;

//Bus edit
const uint16_t mu16_BUS_DEFAULT_SIGNAL_LENGTH = 1U;

//Tooltip
const int32_t ms32_TOOL_TIP_DELAY = 100;
const uint32_t mu32_TOOL_TIP_MAXIMUM_ITEMS = 3;

//Message box default size
const int32_t ms32_MESSAGE_BOX_DEFAULT_WIDTH = 600;

//Report popup size
const QSize mc_POPUP_REPORT_SIZE(1210, 790);

//Icon Size
const QSize mc_ICON_SIZE_24 = QSize(24, 24);
const QSize mc_ICON_SIZE_20 = QSize(20, 20);
const QSize mc_ICON_SIZE_16 = QSize(16, 16);

// Paths
const QString mc_DLL_PATH_PEAK = "STW_dlls\\stwpeak2\\stwpeak2.dll";
const QString mc_DLL_PATH_VECTOR = "STW_dlls\\stwvec32\\stwvec32.dll";

// Identifier of update package subsections or items
const uint32_t mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_DATABLOCK = 0U;
const uint32_t mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PARAMSET = 1U;
const uint32_t mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE = 2U;
const uint32_t mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_PEM = 3U;

// Highest known generated code structure version
const uint16_t mu16_HIGHEST_KNOWN_CODE_STRUCTURE_VERSION = 6U;

// File check interval (for  Update package file check)
const uint32_t mu32_FILE_CHECK_INTERVAL = 500U;
// File check interval (for  CAN Monitor database check)
const uint32_t mu32_DATABASE_CHECK_INTERVAL = 2000U;

// HTML styling
const QString mc_REPORT_HEADLINE_HTML_TAG_START = "<h3>";
const QString mc_REPORT_HEADLINE_HTML_TAG_END = "</h3>";
const QString mc_REPORT_HIGHLIGHT_TAG_START = "<span style=\" font-weight: bold;\">";
const QString mc_REPORT_HIGHLIGHT_TAG_END = "</span>";

//User roles
//----------------------------------------------------------------------------------------------------------------------
//Generic string for additional information (type: QString)
const int32_t ms32_USER_ROLE_ADDITIONAL_INFORMATION = static_cast<int32_t>(Qt::UserRole) + 10;
//Tooltip (standard interface)
//Type: QString
const int32_t ms32_USER_ROLE_TOOL_TIP_HEADING = static_cast<int32_t>(Qt::UserRole) + 20;
//Type: QString
const int32_t ms32_USER_ROLE_TOOL_TIP_CONTENT = static_cast<int32_t>(Qt::UserRole) + 21;
//Type: bool
const int32_t ms32_USER_ROLE_TOOL_TIP_TYPE = static_cast<int32_t>(Qt::UserRole) + 22;
//Pixmap for dark/bright switch (type: QPixmap)
const int32_t ms32_USER_ROLE_PIXMAP_DARK_MODE = static_cast<int32_t>(Qt::UserRole) + 30;
const int32_t ms32_USER_ROLE_PIXMAP_BRIGHT_MODE = static_cast<int32_t>(Qt::UserRole) + 31;
//Used for custom SVG icon painting (type: QStringList)
//Current: either two entries -> one icon for all, or expecting three entries
//0: Simple number for icon size, e.g. "16" for 16x16 or "24" for 24x24
//1:
//Case size two: Always use this icon path
//Case size three: Enabled icon path
//2 (Optional): Disabled icon path
const int32_t ms32_USER_ROLE_ICON = static_cast<int32_t>(Qt::UserRole) + 32;
//Generic flag for a conditional value (type: bool)
const int32_t ms32_USER_ROLE_CONDITIONAL_VALUE = static_cast<int32_t>(Qt::UserRole) + 50;
//Flag to use for error reporting/error styling (type: bool, default: false)
const int32_t ms32_USER_ROLE_ERROR = static_cast<int32_t>(Qt::UserRole) + 60;
//Generic table interface: Specify interaction element (type: E_USER_ROLE_INTERACTION_ELEMENT_VALUE (converted to int),
// default: eURIEL_NONE)
const int32_t ms32_USER_ROLE_INTERACTION_ELEMENT_TYPE = static_cast<int32_t>(Qt::UserRole) + 70;
enum E_UserRoleInteractionElementValue
{
   eURIEL_NONE = 0,              //Qt default if any or in our case: override by manual implementation
   eURIEL_LINE_EDIT,             //input: any string (also integer as string), output: same type, supports: min & max
                                 // handling
   eURIEL_COMBO_BOX,             //input: integer, output: integer, supports: combo box values and strings list
   eURIEL_GENERIC_SPIN_BOX,      //input: integer or floating point, output: same type, only works with min & max
                                 // handling
   eURIEL_MULTI_SELECT_COMBO_BOX //input: QBitArray, output: QBitArray, supports: multi select combo box strings list
};

//Generic table interface: Minimum value to use if flag set (type: any number)
const int32_t ms32_USER_ROLE_INTERACTION_MINIMUM_VALUE = static_cast<int32_t>(Qt::UserRole) + 71;
//Generic table interface: Maximum value to use if flag set (type: any number)
const int32_t ms32_USER_ROLE_INTERACTION_MAXIMUM_VALUE = static_cast<int32_t>(Qt::UserRole) + 72;
//Generic table interface: Flag to see if item has link interaction handling (type: bool, default: false)
const int32_t ms32_USER_ROLE_INTERACTION_IS_LINK = static_cast<int32_t>(Qt::UserRole) + 73;
//Generic table interface: Flag to see if cell requires a minimum value while interacting (type: bool, default: false)
const int32_t ms32_USER_ROLE_INTERACTION_USE_MIN_VALUE = static_cast<int32_t>(Qt::UserRole) + 74;
//Generic table interface: Flag to see if cell requires a maximum value while interacting (type: bool, default: false)
const int32_t ms32_USER_ROLE_INTERACTION_USE_MAX_VALUE = static_cast<int32_t>(Qt::UserRole) + 75;
//Generic table interface: Strings for combo box (type: QStringList, default: none)
const int32_t ms32_USER_ROLE_INTERACTION_COMBO_BOX_STRINGS_LIST = static_cast<int32_t>(Qt::UserRole) +
                                                                  76;
//Generic table interface: Values for combo box (type: QStringList, default: none)
const int32_t ms32_USER_ROLE_INTERACTION_COMBO_BOX_VALUES_LIST = static_cast<int32_t>(Qt::UserRole) +
                                                                 77;
//Generic table interface: Flag to use for editable property of combo box (type: bool, default: false)
const int32_t ms32_USER_ROLE_INTERACTION_COMBO_BOX_EDITABLE = static_cast<int32_t>(Qt::UserRole) + 78;
//Generic table interface: Strings for multi-selection combo box (type: QStringList, default: none)
const int32_t ms32_USER_ROLE_INTERACTION_MULTI_SELECT_COMBO_BOX_STRINGS_LIST =
   static_cast<int32_t>(Qt::UserRole) + 79;
//Generic table interface: Parameters for spin box (type: QStringList, default: none)
//Expected 4 elements:
//0: Min of type C_OscNodeDataPoolContent encoded as string
//1: Max of type C_OscNodeDataPoolContent encoded as string
//2: Factor (float64) encoded as string
//3: Offset (float64) encoded as string
const int32_t ms32_USER_ROLE_INTERACTION_GENERIC_SPIN_BOX_PARAMETERS_LIST =
   static_cast<int32_t>(Qt::UserRole) + 80;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // CONSTANTS_HPP
