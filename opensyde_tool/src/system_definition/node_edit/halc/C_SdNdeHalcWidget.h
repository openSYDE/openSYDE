//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing hardware configuration.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCWIDGET_H
#define C_SDNDEHALCWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_OSCHalcDef.h"
#include "C_OSCHalcConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcWidget(void);

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void SetNode(const stw_types::uint32 ou32_NodeIndex);
   void Save(void);
   void ShowChannel(const stw_types::uint32 ou32_DomainIndex, const stw_types::sint32 os32_ChannelIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;
   void SigHalcDataPoolChanged(void) const;

protected:
   virtual void hideEvent(QHideEvent * const opc_Event) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;

private:
   Ui::C_SdNdeHalcWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   bool mq_HalcDescriptionSelected;

   static const QString mhc_CONFIG_FILE_TYPE;

   //Avoid call
   C_SdNdeHalcWidget(const C_SdNdeHalcWidget &);
   C_SdNdeHalcWidget & operator =(const C_SdNdeHalcWidget &);

   void m_DisableButtons(const bool oq_Active) const;
   void m_OnImportConfigClicked(void);
   void m_OnExportConfigClicked (void);
   void m_OnCleanUpClicked(void) const;
   void m_OnSelectClicked(void);
   void m_OnUpdateClicked(void);
   void m_RunDatapoolMagician(void);
   void m_OnOverviewClicked(void) const;
   void m_ShowOverview(const bool oq_Show) const;
   void m_OnChannelSelected(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                            const bool oq_UseChannelIndex) const;
   void m_OnChannelUpdate(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                          const bool oq_UseChannelIndex) const;
   bool m_LoadHalcDefinitionFile(stw_opensyde_core::C_OSCHalcConfig & orc_HalcConfig, QString & orc_HalcDefPath);
   void m_UpdateNodeData(void) const;
   void m_UpdatePinCount(void) const;
   void m_UpdateDisplayedData(void) const;

   static QString mh_GetDialogPath(const QString & orc_LastKnownPath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
