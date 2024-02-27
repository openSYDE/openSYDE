//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition topology

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDTOPOLOGYWIDGET_HPP
#define C_SDTOPOLOGYWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_NagUseCaseWidget.hpp"
#include "C_OgeWiHover.hpp"
#include "C_SdTopologyScene.hpp"
#include "C_OgeWiFixPosition.hpp"
#include "C_SdTopologyToolbox.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdTopologyWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdTopologyWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdTopologyWidget(QWidget * const opc_Parent = NULL);
   ~C_SdTopologyWidget() override;

   void SetParentHook(QWidget * const opc_Parent);
   void SaveToData(void) const;

   void HideAll(const bool oq_Hide);
   void PrepareToClose(void) const;

   C_SdTopologyScene * GetScene(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode, const uint32_t ou32_Index,
                      const QString & orc_Name, const QString & orc_SubSubItemName, const uint32_t ou32_Flag,
                      const bool oq_ChangeUseCase = false);
   void SigChanged(void);
   void SigNodeDeleted(const uint32_t ou32_Index);
   void SigBusDeleted(const uint32_t ou32_Index);
   void SigNodeChanged(const uint32_t ou32_Index);
   void SigBusChanged(const uint32_t ou32_Index);
   bool SigOpenTsp();
   void SigErrorChange(void);

private:
   //Avoid call
   C_SdTopologyWidget(const C_SdTopologyWidget &);
   C_SdTopologyWidget & operator =(const C_SdTopologyWidget &) &;

   void m_WiFixPosMaxBtnClicked(void);
   void m_WiHoverMinBtnClicked(void);

   Ui::C_SdTopologyWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeWiHover * mpc_Toolbox;
   stw::opensyde_gui_elements::C_OgeWiFixPosition * mpc_FixMinimizedToolbox;

   C_SdTopologyScene * mpc_Scene;

   static const int32_t mhs32_WIDGET_BORDER;
   static const int32_t mhs32_TOOLBOX_INIT_POS_Y;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // C_SYSDEFTOPOLOGYWIDGET_HPP
