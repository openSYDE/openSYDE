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

#include "stwtypes.h"

#include "C_GiBiTextElement.h"
#include "C_GiBiCustomToolTip.h"
#include "C_PuiSdDataElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiTextElementBus :
   public C_GiBiTextElement,
   public stw_opensyde_gui_logic::C_PuiSdDataElement,
   public C_GiBiCustomToolTip
{
   Q_OBJECT

public:
   C_GiTextElementBus(const stw_types::sint32 & ors32_Index, const stw_types::uint64 & oru64_ID,
                      QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiTextElementBus();

   void SetBusIndex(const stw_types::uint32 ou32_BusIndex);
   stw_types::uint32 GetBusIndex(void) const;

   virtual stw_types::sintn type(void) const override;

   virtual void LoadData(void) override;
   virtual void UpdateData(void) override;
   virtual void DeleteData(void) override;

   virtual void GenerateHint(void) override;

   virtual void SetFontColor(const QColor & orc_Color) override;

   void SetErrorState(const bool oq_Error);
   void SetDisabledLook(const bool oq_Disabled);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;
   //lint -restore

   void HandleMouseMoveToolTip(const QPointF & orc_ScenePos);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideToolTip(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event) override;
   //lint -restore

   bool mq_ErrorIconHovered;

private:
   stw_types::uint32 mu32_BusIndex;
   bool mq_ErrorState;
   bool mq_ErrorResized;
   QPixmap mc_ErrorIcon;

   QColor mc_OriginalColor;
   QSvgRenderer * mpc_SvgRenderer;

   QRectF m_GetErrorIconRect(void) const;

   //Avoid call
   C_GiTextElementBus(const C_GiTextElementBus &);
   C_GiTextElementBus & operator =(const C_GiTextElementBus &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
