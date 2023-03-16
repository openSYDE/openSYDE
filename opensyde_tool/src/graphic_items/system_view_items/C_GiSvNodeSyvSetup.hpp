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
#include "stwtypes.hpp"

#include "C_GiSvNodeSyvBase.hpp"
#include "C_GiCheckBox.hpp"
#include "C_GiRectPixmap.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiSvNodeSyvSetup :
   public C_GiSvNodeSyvBase
{
   Q_OBJECT

public:
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   C_GiSvNodeSyvSetup(const uint32_t ou32_ViewIndex, const int32_t & ors32_NodeIndex, const uint64_t & oru64_Id,
                      const float64_t & orf64_Width = mhf64_MIN_WIDTH_NODE,
                      const float64_t & orf64_Height = mhf64_MIN_HEIGHT_NODE, QGraphicsItem * const opc_Parent = NULL);

   void SetViewConnected(const bool oq_Connected) override;
   bool IsViewConnected(void) const;
   void SetEditMode(const bool oq_Active);

   void UpdateData(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigViewConnectionChanged(void);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_GiSvNodeSyvSetup(const C_GiSvNodeSyvSetup &);
   C_GiSvNodeSyvSetup & operator =(const C_GiSvNodeSyvSetup &) &; //lint !e1511 //we want to hide the base

   void m_InitCheckBox(void);

   C_GiCheckBox * mpc_CheckBox;
   bool mq_EditMode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
