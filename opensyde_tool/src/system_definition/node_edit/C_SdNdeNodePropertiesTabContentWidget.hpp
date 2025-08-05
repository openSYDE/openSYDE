//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for node properties tab content
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDENODEPROPERTIESTABCONTENTWIDGET_HPP
#define C_SDNDENODEPROPERTIESTABCONTENTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeNodePropertiesTabContentWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeNodePropertiesTabContentWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeNodePropertiesTabContentWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeNodePropertiesTabContentWidget(void) override;

   void SetNodeIndex(const uint32_t ou32_NodeIndex);
   void UpdateApplications(void) const;
   void SaveToData(void) const;
   void SelectName(void) const;
   void ShowApplication(const uint32_t ou32_ApplicationIndex) const;
   void AddFromTsp(const bool oq_IsNewNode);
   void SaveUserSettings(void);
   void LoadUserSettings(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigHalcLoadedFromTsp(void);
   void SigOwnedDataPoolsChanged(void) const;
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
   void SigBusBitrateClicked(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void SigNodePropChanged(void);
   void SigNodeXappSupportChanged(void);
   void SigUpdateTrigger(void);

private:
   Ui::C_SdNdeNodePropertiesTabContentWidget * mpc_Ui;
   void m_UpdateTrigger(const uint32_t ou32_NodeIndex, const bool oq_OnlyUpdateProperties);

   //Avoid call
   C_SdNdeNodePropertiesTabContentWidget(const C_SdNdeNodePropertiesTabContentWidget &);
   C_SdNdeNodePropertiesTabContentWidget & operator =(const C_SdNdeNodePropertiesTabContentWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
