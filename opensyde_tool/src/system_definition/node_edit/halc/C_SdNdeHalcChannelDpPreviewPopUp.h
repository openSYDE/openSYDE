//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up widget for DP preview
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELDPPREVIEWPOPUP_H
#define C_SDNDEHALCCHANNELDPPREVIEWPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCHalcDefDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcChannelDpPreviewPopUp;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelDpPreviewPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcChannelDpPreviewPopUp(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DomainIndex,
                                             const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex);
   ~C_SdNdeHalcChannelDpPreviewPopUp(void);

   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeHalcChannelDpPreviewPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);

   void m_UpdateContent(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
                        const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex) const;
   static void mh_AddDpSection(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
                               const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                               QString & orc_Text);
   static void mh_AddDeSection(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DomainIndex,
                               const stw_types::uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                               QString & orc_Text);
   static void mh_AddListSection(const std::vector<stw_opensyde_core::C_OSCHalcDefStruct> & orc_Definition,
                                 const std::vector<stw_types::uint32> & orc_Indices,
                                 const stw_scl::C_SCLString & orc_ListName,
                                 const stw_scl::C_SCLString & orc_DomainSingularName, QString & orc_Text,
                                 const stw_opensyde_core::C_OSCHalcDefDomain & orc_Domain,
                                 const bool oq_AddSpecialVars = false);
   static void mh_AddListEntry(const stw_scl::C_SCLString & orc_ListName, bool & orq_ListAdded,
                               const stw_scl::C_SCLString & orc_VarName, const stw_scl::C_SCLString & orc_VarComment,
                               QString & orc_Text);

   //Avoid call
   C_SdNdeHalcChannelDpPreviewPopUp(const C_SdNdeHalcChannelDpPreviewPopUp &);
   C_SdNdeHalcChannelDpPreviewPopUp & operator =(const C_SdNdeHalcChannelDpPreviewPopUp &);

   static const stw_types::sintn mhsn_TableSpacing;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
