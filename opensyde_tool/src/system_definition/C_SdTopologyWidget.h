//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition topology

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_SDTOPOLOGYWIDGET_H
#define C_SDTOPOLOGYWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include "C_NagUseCaseWidget.h"
#include "C_OgeWiHover.h"
#include "C_SdTopologyScene.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdTopologyWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdTopologyWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdTopologyWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdTopologyWidget();

   void SetParentHook(QWidget * const opc_Parent);
   void SaveToData(void) const;

   void HideAll(const bool oq_Hide);
   void PrepareToClose(void) const;

   C_SdTopologyScene * GetScene(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag);
   void SigChanged(void);
   void SigNodeDeleted(const stw_types::uint32 ou32_Index);
   void SigBusDeleted(const stw_types::uint32 ou32_Index);
   void SigNodeChanged(const stw_types::uint32 ou32_Index);
   void SigBusChanged(const stw_types::uint32 ou32_Index);

private:
   //Avoid call
   C_SdTopologyWidget(const C_SdTopologyWidget &);
   C_SdTopologyWidget & operator =(const C_SdTopologyWidget &);

   Ui::C_SdTopologyWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;

   C_SdTopologyScene * mpc_Scene;

   static const stw_types::sintn mhsn_WidgetBorder;
   static const stw_types::sintn mhsn_ToolboxInitPosY;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif // C_SYSDEFTOPOLOGYWIDGET_H
