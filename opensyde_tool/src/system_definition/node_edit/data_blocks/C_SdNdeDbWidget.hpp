//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an application in the application list (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBWIDGET_H
#define C_SDNDEDBWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgeLabGroupItem.hpp"
#include "C_OscNodeApplication.hpp"

namespace Ui
{
class C_SdNdeDbWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbWidget(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                            QWidget * const opc_Parent = NULL);
   ~C_SdNdeDbWidget() override;

   void InitStaticNames(void) const;
   void UpdateApplication(void);
   void UpdateApplicationIndex(const uint32_t ou32_ApplicationIndex);
   uint32_t GetNodeIndex(void) const;
   uint32_t GetApplicationIndex(void) const;
   void CheckProcessIdError(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDelete(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void SigCheckNodeId(void);
   void SigOwnedDataPoolsChanged(void) const;

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeDbWidget(const C_SdNdeDbWidget &);
   C_SdNdeDbWidget & operator =(const C_SdNdeDbWidget &) &;

   void m_LoadData(void);
   void m_OnEdit(void);
   void m_OnDelete(void);
   void m_OnOpenIdeClicked(void);
   void m_OnCommentToggled(const bool oq_Checked);
   void m_UnassignAllAssociatedDataPools(void) const;
   stw::opensyde_core::C_OscNodeApplication::E_Type m_GetType(void) const;
   uint32_t m_CountAllAssociatedDataPools(void) const;
   QString m_GetAllAssociatedDataPoolNames(void) const;
   QString m_GetAllOutputFiles(void) const;

   Ui::C_SdNdeDbWidget * mpc_Ui;
   uint32_t mu32_ApplicationIndex;
   const uint32_t mu32_NodeIndex;
};
}
}

#endif
