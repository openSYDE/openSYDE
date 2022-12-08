//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics view for the message layout viewer (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMLVGRAPHICSVIEW_H
#define C_SDBUEMLVGRAPHICSVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsView>

#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvGraphicsView :
   public QGraphicsView
{
   Q_OBJECT

public:
   C_SdBueMlvGraphicsView(QWidget * const opc_Parent = NULL);
   ~C_SdBueMlvGraphicsView() override;

   // tool tip
   void ShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading, const QString & orc_Content,
                    const bool oq_Error);
   void HideToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowToolTip(const QPointF & orc_ScenePos);

protected:
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueMlvGraphicsView(const C_SdBueMlvGraphicsView &);
   C_SdBueMlvGraphicsView & operator =(const C_SdBueMlvGraphicsView &) &;

   // tool tip
   C_NagToolTip mc_ToolTip;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
