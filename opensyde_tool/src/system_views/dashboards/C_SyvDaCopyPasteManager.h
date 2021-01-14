//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle most parts of copy paste implementation for scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACOPYPASTEMANAGER_H
#define C_SYVDACOPYPASTEMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SyvDaDashboardSnapshot.h"
#include "C_SebBaseCopyPasteManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaCopyPasteManager :
   public C_SebBaseCopyPasteManager
{
public:
   C_SyvDaCopyPasteManager(void);
   virtual ~C_SyvDaCopyPasteManager(void);
   virtual const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) override;
   void PrepareCopyFromSceneToManager(const stw_types::uint32 ou32_ViewIndex,
                                      const stw_types::uint32 ou32_DashboardIndex);
   virtual void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                       const QMap<const QGraphicsItem *, stw_types::float64> & orc_NormalizedZValues)
   override;
   virtual bool CheckValidContentAndPrepareData(void) override;

   static void h_AdaptCopyDataForPaste(stw_opensyde_gui_logic::C_PuiSvDashboard & orc_CopyData,
                                       const stw_types::uint32 ou32_ViewIndex, QWidget * const opc_Parent);

   static const QString hc_ClipBoardBaseTagName;

protected:
   virtual void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data) override;

private:
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_DashboardIndex;
   C_SyvDaDashboardSnapshot mc_LastKnownData;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
