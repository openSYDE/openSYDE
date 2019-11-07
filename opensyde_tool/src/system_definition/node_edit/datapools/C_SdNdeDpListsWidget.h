//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for complete lists handling (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTSWIDGET_H
#define C_SDNDEDPLISTSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListsWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpListsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListsWidget(void);

   void InitText(void) const;

   void Clear(void) const;
   void SetDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
   void OpenDetail(const stw_types::sint32 os32_ListIndex, const stw_types::sint32 os32_DataElementIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(const bool & orq_Error);
   void SigSizeChange(void);
   void SigChanged(void);
   void SigSave(void);
   void SigSaveAs(void);

private:
   //Avoid call
   C_SdNdeDpListsWidget(const C_SdNdeDpListsWidget &);
   C_SdNdeDpListsWidget & operator =(const C_SdNdeDpListsWidget &);

   Ui::C_SdNdeDpListsWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_LastKnownListSelectionCount;
   stw_types::uint32 mu32_LastKnownTableSelectionCount;

   void m_InitButtonIcons(void) const;
   void m_HandleErrorChange(const bool & orq_Error);
   void m_HandleSizeChange(void);
   void m_HandleSelection(const stw_types::uint32 & oru32_Count, const bool & orq_List);
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveDownActive, const bool & orq_MoveUpActive) const;
   void m_HandleChanged(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
