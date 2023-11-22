//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for style setup to customize graphic elements (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILIBUSSTYLESETUPWIDGET_HPP
#define C_GILIBUSSTYLESETUPWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_SdTopologyScene.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_GiSyBaseWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyBaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_GiSyBaseWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_Name = "",
                             const bool oq_DarkMode = false);
   ~C_GiSyBaseWidget(void) override;

   void InitStaticNames(void);
   void SetWidget(QWidget * const opc_Widget);
   C_SdTopologyScene * GetPreviewScene(void);
   static QSize h_GetSceneViewSize(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_GiSyBaseWidget(const C_GiSyBaseWidget &);
   C_GiSyBaseWidget & operator =(const C_GiSyBaseWidget &) &;

   Ui::C_GiSyBaseWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   C_SdTopologyScene * mpc_Scene;

   void m_OkClicked(void);
   void m_CancelClicked(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
