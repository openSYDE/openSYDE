//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Fix position widget in material design

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIFIXPOSITION_H
#define C_OGEWIFIXPOSITION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_OgeWiFixPosition;
}

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OgeWiFixPosition :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_OgeWiFixPosition(QString oc_Title, QString oc_IconPath, const QRect & orc_Geometry,
                               QWidget * const opc_Parent = NULL);
   ~C_OgeWiFixPosition(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigWiFixPosMaxBtnClicked(void);

private:
   Ui::C_OgeWiFixPosition * mpc_Ui;

   //Avoid call
   C_OgeWiFixPosition(const C_OgeWiFixPosition &);
   C_OgeWiFixPosition & operator = (const C_OgeWiFixPosition &);

   void m_ApplyShadow(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
