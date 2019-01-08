//-----------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for node applications (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.01.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDBLISTWIDGET_H
#define C_SDNDEDBLISTWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QListWidget>
#include <QDropEvent>

#include "stwtypes.h"
#include "C_SdNdeDbListDelegate.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDbListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDbListWidget();

   void SetIndex(const stw_types::uint32 ou32_NodeIndex);
   void AddApplication(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ApplicationIndex);
   void UpdateApplications(void) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   //lint -restore

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDuplicate(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ApplicationIndex);
   void SigDelete(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ApplicationIndex);
   void SigAppDisplay(void);
   void SigErrorChange(void) const;
   void SigReloadDataPools(void);
   void SigCheckDataPoolInteraction(void);
   void SigOpenDataPool(const stw_types::uint32 ou32_DataPoolIndex);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   //lint -restore

private:
   //Avoid call
   C_SdNdeDbListWidget(const C_SdNdeDbListWidget &);
   C_SdNdeDbListWidget & operator =(const C_SdNdeDbListWidget &);

   void m_MoveApplication(const stw_types::sintn osn_SourceIndex, const stw_types::sintn osn_TargetIndex);
   void m_UpdateApplicationIndexes(void) const;
   void m_CheckNodeId(void) const;

   stw_opensyde_gui_logic::C_SdNdeDbListDelegate mc_Delegate;

   stw_types::uint32 mu32_NodeIndex;
   stw_types::sintn msn_DragItemIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
