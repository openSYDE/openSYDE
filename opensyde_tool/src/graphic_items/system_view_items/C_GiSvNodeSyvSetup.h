//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view setup specific visualization and functionality of a node. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GINODESYVSETUP_H
#define C_GINODESYVSETUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_GiSvNodeSyvBase.h"
#include "C_GiCheckBox.h"
#include "C_GiRectPixmap.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeSyvSetup :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   C_GiSvNodeSyvSetup(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 & ors32_NodeIndex,
                      const stw_types::uint64 & oru64_ID, const stw_types::float64 & orf64_Width = mhf64_MIN_WIDTH_NODE,
                      const stw_types::float64 & orf64_Height = mhf64_MIN_HEIGHT_NODE,
                      QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiSvNodeSyvSetup();

   virtual void SetViewConnected(const bool oq_Connected) override;
   bool IsViewConnected(void) const;
   void SetEditMode(const bool oq_Active);

   virtual void UpdateData(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigViewConnectionChanged(void);

protected:
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_GiSvNodeSyvSetup(const C_GiSvNodeSyvSetup &);
   C_GiSvNodeSyvSetup & operator =(const C_GiSvNodeSyvSetup &); //lint !e1511 //we want to hide the base

   void m_InitCheckBox(void);

   C_GiCheckBox * mpc_CheckBox;
   bool mq_EditMode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
