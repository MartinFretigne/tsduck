//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2020, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of a Program Association Table (PAT)
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractLongTable.h"
#include "tsMPEG.h"

namespace ts {
    //!
    //! Representation of a Program Association Table (PAT).
    //! @ingroup table
    //!
    class TSDUCKDLL PAT : public AbstractLongTable
    {
    public:
        //!
        //! List of PMT PID's, indexed by service_id.
        //!
        typedef std::map<uint16_t, PID> ServiceMap;

        // Public members:
        uint16_t   ts_id;    //!< Transport stream id.
        PID        nit_pid;  //!< PID for NIT.
        ServiceMap pmts;     //!< Map of PMT PID's: key=service_id, value=pmt_pid.

        //!
        //! Default constructor.
        //! @param [in] version Table version number.
        //! @param [in] is_current True if table is current, false if table is next.
        //! @param [in] ts_id Transport stream identifier.
        //! @param [in] nit_pid PID of the NIT. Default: DVB-defined PID for NIT.
        //!
        PAT(uint8_t  version = 0,
            bool     is_current = true,
            uint16_t ts_id = 0,
            PID      nit_pid = PID_NIT);

        //!
        //! Constructor from a binary table.
        //! @param [in,out] duck TSDuck execution context.
        //! @param [in] table Binary table to deserialize.
        //!
        PAT(DuckContext& duck, const BinaryTable& table);

        // Inherited methods
        virtual void fromXML(DuckContext&, const xml::Element*) override;
        DeclareDisplaySection();

    protected:
        // Inherited methods
        virtual void serializeContent(DuckContext&, BinaryTable&) const override;
        virtual void deserializeContent(DuckContext&, const BinaryTable&) override;
        virtual void buildXML(DuckContext&, xml::Element*) const override;
    };
}
