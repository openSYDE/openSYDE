//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text element graphics item for showing bus name  (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GITEXTELEMENTBUS_H
#define C_GITEXTELEMENTBUS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPixmap>

#include "stwtypes.hpp"

#include "C_GiBiTextElement.hpp"
#include "C_GiBiCustomToolTip.hpp"
#include "C_PuiSdDataElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiTextElementBus :
   public C_GiBiTextElement,
   public stw::opensyde_gui_logic::C_PuiSdDataElement,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   C_GiTextElementBus(const int32_t & ors32_Index, const uint64_t & oru64_Id, QGraphicsItem * const opc_Parent = NULL);
   ~C_GiTextElementBus() override;

   void SetBusIndex(const uint32_t ou32_BusIndex);
   uint32_t GetBusIndex(void) const;

   int32_t type(void) const override;

   void LoadData(void) override;
   void UpdateData(void) override;
   void DeleteData(void) override;

   void GenerateHint(void) override;

   void SetFontColor(const QColor & orc_Color) override;

   void SetErrorState(const bool oq_Error);
   void SetDisabledLook(const bool oq_Disabled);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

   void HandleMouseMoveToolTip(const QPointF & orc_ScenePos);

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

protected:
   void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;

   bool mq_ErrorIconHovered;

private:
   uint32_t mu32_BusIndex;
   bool mq_ErrorState;
   bool mq_ErrorResized;
   QPixmap mc_ErrorIcon;

   QColor mc_OriginalColor;
   QSvgRenderer * mpc_SvgRenderer;

   QRectF m_GetErrorIconRect(void) const;

   //Avoid call
   C_GiTextElementBus(const C_GiTextElementBus &);
   C_GiTextElementBus & operator =(const C_GiTextElementBus &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
