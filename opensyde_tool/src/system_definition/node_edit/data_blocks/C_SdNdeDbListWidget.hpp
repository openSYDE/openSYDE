//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for node applications (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBLISTWIDGET_H
#define C_SDNDEDBLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListWidget>
#include <QDropEvent>

#include "stwtypes.hpp"
#include "C_SdNdeDbListDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbListWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDbListWidget() override;

   void SetIndex(const uint32_t ou32_NodeIndex);
   void AddApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void UpdateApplications(void) const;

   void dropEvent(QDropEvent * const opc_Event) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDelete(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void SigAppDisplay(void);
   void SigErrorChange(void) const;
   void SigOwnedDataPoolsChanged(void) const;

protected:
   void startDrag(const Qt::DropActions oc_SupportedActions) override;

private:
   //Avoid call
   C_SdNdeDbListWidget(const C_SdNdeDbListWidget &);
   C_SdNdeDbListWidget & operator =(const C_SdNdeDbListWidget &) &;

   void m_MoveApplication(const int32_t os32_SourceIndex, const int32_t os32_TargetIndex);
   void m_UpdateApplicationIndexes(void) const;
   void m_CheckNodeId(void) const;

   stw::opensyde_gui_logic::C_SdNdeDbListDelegate mc_Delegate;

   uint32_t mu32_NodeIndex;
   int32_t ms32_DragItemIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
