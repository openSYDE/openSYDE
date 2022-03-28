//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition topology

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDTOPOLOGYWIDGET_H
#define C_SDTOPOLOGYWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_NagUseCaseWidget.h"
#include "C_OgeWiHover.h"
#include "C_SdTopologyScene.h"
#include "C_OgeWiFixPosition.h"
#include "C_SdTopologyToolbox.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdTopologyWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
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
                      const stw_types::uint32 ou32_Flag, const bool oq_ChangeUseCase = false);
   void SigChanged(void);
   void SigNodeDeleted(const stw_types::uint32 ou32_Index);
   void SigBusDeleted(const stw_types::uint32 ou32_Index);
   void SigNodeChanged(const stw_types::uint32 ou32_Index);
   void SigBusChanged(const stw_types::uint32 ou32_Index);
   void SigErrorChange(void);

private:
   //Avoid call
   C_SdTopologyWidget(const C_SdTopologyWidget &);
   C_SdTopologyWidget & operator =(const C_SdTopologyWidget &);

   void m_WiFixPosMaxBtnClicked(void);
   void m_WiHoverMinBtnClicked(void);

   Ui::C_SdTopologyWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;
   stw_opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedToolbox;

   C_SdTopologyScene * mpc_Scene;

   static const stw_types::sintn mhsn_WIDGET_BORDER;
   static const stw_types::sintn mhsn_TOOLBOX_INIT_POS_Y;

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif // C_SYSDEFTOPOLOGYWIDGET_H
