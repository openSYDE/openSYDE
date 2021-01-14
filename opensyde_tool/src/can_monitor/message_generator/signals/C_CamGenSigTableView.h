//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for message generator signals table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENSIGTABLEVIEW_H
#define C_CAMGENSIGTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_OgePubIconOnly.h"
#include "C_TblViewToolTipBase.h"
#include "C_CamGenSigTableModel.h"
#include "C_CamGenSigTableDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenSigTableView :
   public C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_CamGenSigTableView(QWidget * const opc_Parent = NULL);
   ~C_CamGenSigTableView(void);

   void TriggerSignalReload(void);
   void SetMessage(const stw_types::uint32 ou32_Message);
   void UpdateMessageDLC(const stw_types::uint32 ou32_MessageIndex);

   void SetCurrentColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigUpdateMessageData(const stw_types::uint32 ou32_MessageIndex);
   void SigTriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   C_CamGenSigTableDelegate mc_Delegate;
   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_CamGenSigTableModel mc_Model;
   stw_opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;

   void m_RepositionButtons(void);
   void m_SetPersistentEditorWidgets(void);

   //Avoid call
   C_CamGenSigTableView(const C_CamGenSigTableView &);
   C_CamGenSigTableView & operator =(const C_CamGenSigTableView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
