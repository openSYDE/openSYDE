//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for style setup to customize graphic elements (header)

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.10.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GILIBUSSTYLESETUPWIDGET_H
#define C_GILIBUSSTYLESETUPWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>

#include "stwtypes.h"

#include "C_SdTopologyScene.h"
#include "C_OgePopUpDialog.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_GiSyBaseWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_GiSyBaseWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_GiSyBaseWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const QString & orc_Name = "",
                             const bool oq_DarkMode = false);
   virtual ~C_GiSyBaseWidget(void);

   void InitStaticNames(void);
   void SetWidget(QWidget * const opc_Widget);
   C_SdTopologyScene * GetPreviewScene(void);
   static QSize h_GetSceneViewSize(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_GiSyBaseWidget(const C_GiSyBaseWidget &);
   C_GiSyBaseWidget & operator =(const C_GiSyBaseWidget &);

   Ui::C_GiSyBaseWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   C_SdTopologyScene * mpc_Scene;

   void m_OkClicked(void);
   void m_CancelClicked(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
