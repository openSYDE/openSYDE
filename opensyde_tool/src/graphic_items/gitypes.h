//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header file with type constants of GraphicsItems

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef GITYPES_H
#define GITYPES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

#include <limits>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */
const stw_types::sintn msn_GRAPHICS_ITEM_NODE             = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 1;
const stw_types::sintn msn_GRAPHICS_ITEM_LINE             = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 2;
const stw_types::sintn msn_GRAPHICS_ITEM_BUS              = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 3;
const stw_types::sintn msn_GRAPHICS_ITEM_CANBUS           = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 4;
const stw_types::sintn msn_GRAPHICS_ITEM_ETHERNETBUS      = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 5;
const stw_types::sintn msn_GRAPHICS_ITEM_TEXTELEMENT      = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 6;
const stw_types::sintn msn_GRAPHICS_ITEM_SIMPLELINE       = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 7;
const stw_types::sintn msn_GRAPHICS_ITEM_BUS_CONNECT      = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 8;
const stw_types::sintn msn_GRAPHICS_ITEM_LINE_ARROW       = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 9;
const stw_types::sintn msn_GRAPHICS_ITEM_LINEINTERACTION  = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 10;
const stw_types::sintn msn_GRAPHICS_ITEM_POINTINTERACTION = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 11;
const stw_types::sintn msn_GRAPHICS_ITEM_BOUNDARY         = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 12;
const stw_types::sintn msn_GRAPHICS_ITEM_ARROWCURSORBTN   = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 13;
const stw_types::sintn msn_GRAPHICS_ITEM_TEXT             = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 14;
const stw_types::sintn msn_GRAPHICS_ITEM_IMAGE            = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 15;
const stw_types::sintn msn_GRAPHICS_ITEM_TEXTELEMENT_BUS  = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 16;
const stw_types::sintn msn_GRAPHICS_ITEM_PC               = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 17;
const stw_types::sintn msn_GRAPHICS_ITEM_PC_CONNECTION    = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 18;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_LABEL         = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 19;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_SPIN_BOX      = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 20;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_PROGRESS_BAR  = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 21;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_SLIDER        = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 22;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_TOGGLE        = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 23;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_PIE_CHART     = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 25;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_TABLE         = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 26;
const stw_types::sintn msn_GRAPHICS_ITEM_DB_PARAM         = static_cast<stw_types::sintn>(QGraphicsItem::UserType) + 27;

const stw_types::float64 mf64_ZORDER_INIT_LOW                 = 10000.0;
const stw_types::float64 mf64_ZORDER_INIT_IMAGE               = mf64_ZORDER_INIT_LOW;
const stw_types::float64 mf64_ZORDER_INIT_BOUNDARY            = mf64_ZORDER_INIT_LOW + 1.0;
const stw_types::float64 mf64_ZORDER_INIT_BUS_CONNECT         = mf64_ZORDER_INIT_LOW + 2.0;
const stw_types::float64 mf64_ZORDER_INIT_BUS                 = mf64_ZORDER_INIT_LOW + 3.0;
const stw_types::float64 mf64_ZORDER_INIT_NODE                = mf64_ZORDER_INIT_LOW + 4.0;
const stw_types::float64 mf64_ZORDER_INIT_TEXT_ELEMENT        = mf64_ZORDER_INIT_LOW + 5.0;
const stw_types::float64 mf64_ZORDER_INIT_LINE_ARROW          = mf64_ZORDER_INIT_LOW + 6.0;
const stw_types::float64 mf64_ZORDER_INIT_LABEL_WIDGET        = mf64_ZORDER_INIT_LOW + 7.0;
const stw_types::float64 mf64_ZORDER_INIT_SPIN_BOX_WIDGET     = mf64_ZORDER_INIT_LOW + 8.0;
const stw_types::float64 mf64_ZORDER_INIT_SLIDER_WIDGET       = mf64_ZORDER_INIT_LOW + 9.0;
const stw_types::float64 mf64_ZORDER_INIT_PROGRESS_BAR_WIDGET = mf64_ZORDER_INIT_LOW + 10.0;
const stw_types::float64 mf64_ZORDER_INIT_TOGGLE_WIDGET       = mf64_ZORDER_INIT_LOW + 11.0;
const stw_types::float64 mf64_ZORDER_INIT_CHART_WIDGET        = mf64_ZORDER_INIT_LOW + 12.0;
const stw_types::float64 mf64_ZORDER_INIT_PIE_CHART_WIDGET    = mf64_ZORDER_INIT_LOW + 13.0;
const stw_types::float64 mf64_ZORDER_INIT_TABLE_WIDGET        = mf64_ZORDER_INIT_LOW + 14.0;
const stw_types::float64 mf64_ZORDER_INIT_PARAM_WIDGET        = mf64_ZORDER_INIT_LOW + 15.0;
const stw_types::float64 mf64_ZORDER_INIT_HIGH                = mf64_ZORDER_INIT_PARAM_WIDGET;
const stw_types::float64 mf64_ZORDER_MAX                      = std::numeric_limits<stw_types::float64>::max();

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
