//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2019, Thierry Lelegard
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
//!  Continuity counters analysis and repair.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsMPEG.h"
#include "tsTSPacket.h"
#include "tsReport.h"

namespace ts {
    //!
    //! Continuity counters analysis and repair.
    //! @ingroup mpeg
    //!
    class TSDUCKDLL ContinuityAnalyzer
    {
    public:
        //!
        //! Constructor.
        //! @param [in] pid_filter The set of PID's to analyze or fix.
        //! @param [in] report Where to report discontinuity errors. Drop errors if null.
        //!
        explicit ContinuityAnalyzer(const PIDSet& pid_filter = NoPID, Report* report = nullptr);

        //!
        //! Reset all collected information.
        //! Do not change processing options (display and/or fix errors).
        //!
        void reset();

        //!
        //! Process a constant TS packet.
        //! Can be used only to report discontinuity errors.
        //! @param [in] pkt A transport stream packet.
        //! @return True if the packet has no discontinuity error. False if it has an error.
        //!
        bool feedPacket(const TSPacket& pkt) { return feedPacketInternal(const_cast<TSPacket*>(&pkt), false); }

        //!
        //! Process or modify a TS packet.
        //! @param [in,out] pkt A transport stream packet.
        //! It can be modified only when error fixing or generator mode is activated.
        //! @return True if the packet had no discontinuity error and is unmodified.
        //! False if the packet had an error or was modified.
        //!
        bool feedPacket(TSPacket& pkt) { return feedPacketInternal(&pkt, true); }

        //!
        //! Get the total number of TS packets.
        //! @return The total number of TS packets.
        //!
        PacketCounter totalPackets() const { return _total_packets; }

        //!
        //! Get the number of processed TS packets.
        //! Only packets from selected PID's are counted.
        //! @return The number of processed TS packets.
        //!
        PacketCounter processedPackets() const { return _processed_packets; }

        //!
        //! Get the number of fixed (modified) TS packets.
        //! @return The number of fixed (modified) TS packets.
        //!
        PacketCounter fixCount() const { return _fix_count; }

        //!
        //! Get the number of discontinuity errors.
        //! @return The number of discontinuity errors.
        //!
        PacketCounter errorCount() const { return _error_count; }

        //!
        //! Change the output device to report errors.
        //! @param [in] report Where to report discontinuity errors. Drop errors if null.
        //!
        void setReport(Report* report);

        //!
        //! Change error reporting.
        //! @param [in] display When true, display discontinuity errors.
        //!
        void setDisplay(bool display) { _display_errors = display; }

        //!
        //! Change error fixing.
        //! @param [in] fix When true, fix discontinuity errors.
        //!
        void setFix(bool fix) { _fix_errors = fix; }

        //!
        //! Set generator mode.
        //! When the generator mode is on, the input continuity counters are always ignored.
        //! The output continuity counters are updated to create a continuous stream.
        //! No error is reported.
        //! @param [in] gen When true, activate generator mode.
        //!
        void setGenerator(bool gen) { _generator = gen; }

        //!
        //! Define the severity of messages.
        //! The default severity is Severity::Info.
        //! @param [in] level The severity of each message.
        //!
        void setMessageSeverity(int level) { _severity = level; }

        //!
        //! Define a prefix string to be displayed with each message.
        //! @param [in] prefix The prefix string to be displayed with each message.
        //!
        void setMessagePrefix(const UString& prefix) { _prefix = prefix; }

        //!
        //! Replace the list of PID's to process.
        //! @param [in] pid_filter The list of PID's to process.
        //!
        void setPIDFilter(const PIDSet& pid_filter);

        //!
        //! Add one PID to process.
        //! @param [in] pid The new PID to process.
        //!
        void addPID(PID pid);

        //!
        //! Add several PID's to process.
        //! @param [in] pids The list of new PID's to process.
        //!
        void addPIDs(const PIDSet& pids);

        //!
        //! Remove one PID to process.
        //! @param [in] pid The PID to no longer process.
        //!
        void removePID(PID pid);

        //!
        //! Get the current number of PID's being processed.
        //! @return The current number of PID's being processed.
        //!
        size_t pidCount() const { return _pid_filter.count(); }

        //!
        //! Check if a PID is processed.
        //! @param [in] pid The PID to test.
        //! @return True if @a pid is processed.
        //!
        bool hasPID(PID pid) const;

        //!
        //! Get the first CC in a PID.
        //! @param [in] pid The PID to check.
        //! @return The first CC value in the PID or ts::INVALID_CC when the PID is not filtered.
        //! The first CC in a PID is never modified.
        //!
        uint8_t firstCC(PID pid) const;

        //!
        //! Get the last CC in a PID.
        //! @param [in] pid The PID to check.
        //! @return The last CC value in the PID or ts::INVALID_CC when the PID is not filtered.
        //! This is the output CC value, possibly modified.
        //!
        uint8_t lastCC(PID pid) const;

        //!
        //! Compute the number of missing packets between two continuity counters.
        //! @param [in] cc1 First continuity counter.
        //! @param [in] cc2 Second continuity counter.
        //! @return Number of missing packets between @a cc1 and @a cc2.
        //!
        static int MissingPackets(int cc1, int cc2);

    private:
        // PID analysis state
        class PIDState
        {
        public:
            PIDState();           // Constructor
            uint8_t first_cc;     // First CC value in a PID.
            uint8_t last_cc_in;   // Last input CC value in a PID.
            uint8_t last_cc_out;  // Last output CC value in a PID.
            size_t  dup_count;    // Consecutive duplicate count.
        };

        // A map of PID state, indexed by PID.
        typedef std::map<PID,PIDState> PIDStateMap;

        // Private members.
        Report*       _report;            // Where to report errors, never null.
        int           _severity;          // Severity level for error messages.
        bool          _display_errors;    // Display discontinuity errors.
        bool          _fix_errors;        // Fix discontinuity errors.
        bool          _generator;         // Use generator mode.
        UString       _prefix;            // Message prefix.
        PacketCounter _total_packets;     // Total number of packets.
        PacketCounter _processed_packets; // Number of processed packets.
        PacketCounter _fix_count;         // Number of fixed (modified) packets.
        PacketCounter _error_count;       // Number of discontinuity errors.
        PIDSet        _pid_filter;        // Current set of filtered PID's.
        PIDStateMap   _pid_states;        // State of all PID's.

        // Internal version of feedPacket.
        bool feedPacketInternal(TSPacket* pkt, bool update);

        // Build the first part of an error message.
        UString linePrefix(PID pid) const;

        // Unreachable constructors and operators.
        ContinuityAnalyzer(const ContinuityAnalyzer&) = delete;
        ContinuityAnalyzer& operator=(const ContinuityAnalyzer&) = delete;
    };
}