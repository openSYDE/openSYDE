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
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpListsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListsWidget(void) override;

   void InitText(void) const;

   void Clear(void) const;
   void SetDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void OpenDetail(const int32_t os32_ListIndex, const int32_t os32_DataElementIndex) const;

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
   C_SdNdeDpListsWidget & operator =(const C_SdNdeDpListsWidget &) &;

   Ui::C_SdNdeDpListsWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_LastKnownListSelectionCount;
   uint32_t mu32_LastKnownTableSelectionCount;

   void m_InitButtonIcons(void) const;
   void m_UpdateDpLabel(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const;
   void m_HandleSelection(const uint32_t & oru32_Count, const bool & orq_List);
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveDownActive, const bool & orq_MoveUpActive) const;
   void m_HandleChanged(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
