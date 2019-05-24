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

#include "C_NagToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueMlvGraphicsView :
   public QGraphicsView
{
   Q_OBJECT

public:
   C_SdBueMlvGraphicsView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueMlvGraphicsView();

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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdBueMlvGraphicsView(const C_SdBueMlvGraphicsView &);
   C_SdBueMlvGraphicsView & operator =(const C_SdBueMlvGraphicsView &);

   // tool tip
   C_NagToolTip mc_ToolTip;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
