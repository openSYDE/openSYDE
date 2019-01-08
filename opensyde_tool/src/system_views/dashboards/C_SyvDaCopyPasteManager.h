//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDACOPYPASTEMANAGER_H
#define C_SYVDACOPYPASTEMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SebBaseCopyPasteManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaCopyPasteManager :
   public C_SebBaseCopyPasteManager
{
public:
   C_SyvDaCopyPasteManager(void);
   virtual ~C_SyvDaCopyPasteManager(void);
   virtual const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) override;
   void PrepareCopyFromSceneToManager(const stw_types::uint32 ou32_ViewIndex,
                                      const stw_types::uint32 ou32_DashboardIndex);
   virtual void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems) override;
   virtual bool CheckValidContent(void) const override;

   static const QString hc_ClipBoardBaseTagName;

protected:
   virtual void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data) override;

private:
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_DashboardIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
