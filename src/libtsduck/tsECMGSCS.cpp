//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2017, Thierry Lelegard
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
//
//  DVB SimulCrypt ECMG <=> SCS protocol
//
//----------------------------------------------------------------------------

#include "tsECMGSCS.h"
#include "tsTLVMessageFactory.h"
#include "tsMPEG.h"


// Define protocol singleton instance
tsDefineSingleton (ts::ecmgscs::Protocol);


//----------------------------------------------------------------------------
// Protocol Constructor: Define the syntax of the protocol
//----------------------------------------------------------------------------

ts::ecmgscs::Protocol::Protocol() :
    tlv::Protocol (ts::ecmgscs::CURRENT_VERSION)
{
    // Define the syntax of all commands:
    // add (cmd_tag, param_tag, min_size, max_size, min_count, max_count)

    add (Tags::channel_setup,  Tags::Super_CAS_id,           4, 4, 1, 1);
    add (Tags::channel_setup,  Tags::ECM_channel_id,         2, 2, 1, 1);

    add (Tags::channel_test,   Tags::ECM_channel_id,         2, 2, 1, 1);

    add (Tags::channel_status, Tags::ECM_channel_id,         2, 2, 1, 1);
    add (Tags::channel_status, Tags::section_TSpkt_flag,     1, 1, 1, 1);
    add (Tags::channel_status, Tags::AC_delay_start,         2, 2, 0, 1);
    add (Tags::channel_status, Tags::AC_delay_stop,          2, 2, 0, 1);
    add (Tags::channel_status, Tags::delay_start,            2, 2, 1, 1);
    add (Tags::channel_status, Tags::delay_stop,             2, 2, 1, 1);
    add (Tags::channel_status, Tags::transition_delay_start, 2, 2, 0, 1);
    add (Tags::channel_status, Tags::transition_delay_stop,  2, 2, 0, 1);
    add (Tags::channel_status, Tags::ECM_rep_period,         2, 2, 1, 1);
    add (Tags::channel_status, Tags::max_streams,            2, 2, 1, 1);
    add (Tags::channel_status, Tags::min_CP_duration,        2, 2, 1, 1);
    add (Tags::channel_status, Tags::lead_CW,                1, 1, 1, 1);
    add (Tags::channel_status, Tags::CW_per_msg,             1, 1, 1, 1);
    add (Tags::channel_status, Tags::max_comp_time,          2, 2, 1, 1);

    add (Tags::channel_close,  Tags::ECM_channel_id,         2, 2, 1, 1);

    add (Tags::channel_error,  Tags::ECM_channel_id,         2, 2, 1, 1);
    add (Tags::channel_error,  Tags::error_status,           2, 2, 1, 0xFFFF);
    add (Tags::channel_error,  Tags::error_information,      2, 2, 0, 0xFFFF);

    add (Tags::stream_setup,   Tags::ECM_channel_id,         2, 2, 1, 1);
    add (Tags::stream_setup,   Tags::ECM_stream_id,          2, 2, 1, 1);
    add (Tags::stream_setup,   Tags::ECM_id,                 2, 2, 1, 1);
    add (Tags::stream_setup,   Tags::nominal_CP_duration,    2, 2, 1, 1);

    add (Tags::stream_test,    Tags::ECM_channel_id,         2, 2, 1, 1);
    add (Tags::stream_test,    Tags::ECM_stream_id,          2, 2, 1, 1);

    add (Tags::stream_status,  Tags::ECM_channel_id,                2, 2, 1, 1);
    add (Tags::stream_status,  Tags::ECM_stream_id,                 2, 2, 1, 1);
    add (Tags::stream_status,  Tags::ECM_id,                        2, 2, 1, 1);
    add (Tags::stream_status,  Tags::access_criteria_transfer_mode, 1, 1, 1, 1);

    add (Tags::stream_close_request,  Tags::ECM_channel_id,  2, 2, 1, 1);
    add (Tags::stream_close_request,  Tags::ECM_stream_id,   2, 2, 1, 1);

    add (Tags::stream_close_response, Tags::ECM_channel_id,  2, 2, 1, 1);
    add (Tags::stream_close_response, Tags::ECM_stream_id,   2, 2, 1, 1);

    add (Tags::stream_error,   Tags::ECM_channel_id,         2, 2, 1, 1);
    add (Tags::stream_error,   Tags::ECM_stream_id,          2, 2, 1, 1);
    add (Tags::stream_error,   Tags::error_status,           2, 2, 1, 0xFFFF);
    add (Tags::stream_error,   Tags::error_information,      2, 2, 0, 0xFFFF);

    add (Tags::CW_provision,   Tags::ECM_channel_id,    2, 2,      1, 1);
    add (Tags::CW_provision,   Tags::ECM_stream_id,     2, 2,      1, 1);
    add (Tags::CW_provision,   Tags::CP_number,         2, 2,      1, 1);
    add (Tags::CW_provision,   Tags::CW_encryption,     0, 0xFFFF, 0, 1);
    add (Tags::CW_provision,   Tags::CP_CW_combination, 2+CW_BYTES, 2+CW_BYTES, 1, 0xFFFF);
    add (Tags::CW_provision,   Tags::CP_duration,       2, 2,      0, 1);
    add (Tags::CW_provision,   Tags::access_criteria,   0, 0xFFFF, 0, 1);

    add (Tags::ECM_response,   Tags::ECM_channel_id,    2, 2,      1, 1);
    add (Tags::ECM_response,   Tags::ECM_stream_id,     2, 2,      1, 1);
    add (Tags::ECM_response,   Tags::CP_number,         2, 2,      1, 1);
    add (Tags::ECM_response,   Tags::ECM_datagram,      0, 0xFFFF, 1, 1);
}


//----------------------------------------------------------------------------
// Message factory for the protocol
//----------------------------------------------------------------------------

void ts::ecmgscs::Protocol::factory (const tlv::MessageFactory& fact, tlv::MessagePtr& msg) const
{
    switch (fact.commandTag ()) {
        case Tags::channel_setup:
            msg = new ChannelSetup (fact);
            break;
        case Tags::channel_test:
            msg = new ChannelTest (fact);
            break;
        case Tags::channel_status:
            msg = new ChannelStatus (fact);
            break;
        case Tags::channel_close:
            msg = new ChannelClose (fact);
            break;
        case Tags::channel_error:
            msg = new ChannelError (fact);
            break;
        case Tags::stream_setup:
            msg = new StreamSetup (fact);
            break;
        case Tags::stream_test:
            msg = new StreamTest (fact);
            break;
        case Tags::stream_status:
            msg = new StreamStatus (fact);
            break;
        case Tags::stream_close_request:
            msg = new StreamCloseRequest (fact);
            break;
        case Tags::stream_close_response:
            msg = new StreamCloseResponse (fact);
            break;
        case Tags::stream_error:
            msg = new StreamError (fact);
            break;
        case Tags::CW_provision:
            msg = new CWProvision (fact);
            break;
        case Tags::ECM_response:
            msg = new ECMResponse (fact);
            break;
        default:
            throw tlv::DeserializationInternalError (Format ("ECMG<=>SCS Message 0x%04X unimplemented", fact.commandTag ()));
    }
}


//----------------------------------------------------------------------------
// Create an error response message for a faulty incoming message.
//----------------------------------------------------------------------------

void ts::ecmgscs::Protocol::buildErrorResponse (const tlv::MessageFactory& fact, tlv::MessagePtr& msg) const
{
    // Create a channel_error message
    SafePtr<ChannelError> errmsg (new ChannelError);

    // Try to get an ECM_channel_id from the incoming message.
    try {
        errmsg->channel_id = fact.get<uint16_t> (Tags::ECM_channel_id);
    }
    catch (const tlv::DeserializationInternalError&) {
        errmsg->channel_id = 0;
    }

    // Convert general TLV error code into ECMG <=> SCS error_status
    uint16_t status;
    switch (fact.errorStatus ()) {
        case tlv::OK: // should not happen
        case tlv::InvalidMessage:
            status = Errors::inv_message;
            break;
        case tlv::UnsupportedVersion:
            status = Errors::inv_proto_version;
            break;
        case tlv::UnknownCommandTag:
            status = Errors::inv_message_type;
            break;
        case tlv::UnknownParameterTag:
            status = Errors::inv_param_type;
            break;
        case tlv::InvalidParameterLength:
            status = Errors::inv_param_length;
            break;
        case tlv::InvalidParameterCount:
        case tlv::MissingParameter:
            status = Errors::missing_param;
            break;
    }

    // Copy error_status and error_information into response
    errmsg->error_status.push_back (status);
    errmsg->error_information.push_back (fact.errorInformation ());

    // Transfer ownership of safe ptr
    msg = errmsg.release();
}


//----------------------------------------------------------------------------
// channel_setup
//----------------------------------------------------------------------------

ts::ecmgscs::ChannelSetup::ChannelSetup () :
    ChannelMessage (ecmgscs::Protocol::Instance()->version(), Tags::channel_setup),
    Super_CAS_id   (0)
{
}

ts::ecmgscs::ChannelSetup::ChannelSetup (const tlv::MessageFactory& fact) :
    ChannelMessage (fact.protocolVersion(),
                    fact.commandTag(),
                    fact.get<uint16_t> (Tags::ECM_channel_id)),
    Super_CAS_id   (fact.get<uint32_t> (Tags::Super_CAS_id))
{
}

void ts::ecmgscs::ChannelSetup::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::Super_CAS_id,   Super_CAS_id);
}

std::string ts::ecmgscs::ChannelSetup::dump (size_t indent) const
{
    return Format ("%*schannel_setup (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "Super_CAS_id", Super_CAS_id);
}


//----------------------------------------------------------------------------
// channel_test
//----------------------------------------------------------------------------

ts::ecmgscs::ChannelTest::ChannelTest () :
    ChannelMessage (ecmgscs::Protocol::Instance()->version(), Tags::channel_test)
{
}

ts::ecmgscs::ChannelTest::ChannelTest (const tlv::MessageFactory& fact) :
    ChannelMessage (fact.protocolVersion(),
                    fact.commandTag(),
                    fact.get<uint16_t> (Tags::ECM_channel_id))
{
}

void ts::ecmgscs::ChannelTest::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
}

std::string ts::ecmgscs::ChannelTest::dump (size_t indent) const
{
    return Format ("%*schannel_test (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id);
}


//----------------------------------------------------------------------------
// channel_status
//----------------------------------------------------------------------------

ts::ecmgscs::ChannelStatus::ChannelStatus () :
    ChannelMessage             (ecmgscs::Protocol::Instance()->version(), Tags::channel_status),
    section_TSpkt_flag         (false),
    has_AC_delay_start         (false),
    AC_delay_start             (0),
    has_AC_delay_stop          (false),
    AC_delay_stop              (0),
    delay_start                (0),
    delay_stop                 (0),
    has_transition_delay_start (false),
    transition_delay_start     (0),
    has_transition_delay_stop  (false),
    transition_delay_stop      (0),
    ECM_rep_period             (0),
    max_streams                (0),
    min_CP_duration            (0),
    lead_CW                    (0),
    CW_per_msg                 (0),
    max_comp_time              (0)
{
}

ts::ecmgscs::ChannelStatus::ChannelStatus (const tlv::MessageFactory& fact) :
    ChannelMessage         (fact.protocolVersion(),
                            fact.commandTag(),
                            fact.get<uint16_t> (Tags::ECM_channel_id)),
    section_TSpkt_flag     (fact.get<bool>   (Tags::section_TSpkt_flag)),
    has_AC_delay_start     (1 == fact.count (Tags::AC_delay_start)),
    AC_delay_start         (!has_AC_delay_start ? 0 : fact.get<int16_t> (Tags::AC_delay_start)),
    has_AC_delay_stop      (1 == fact.count (Tags::AC_delay_stop)),
    AC_delay_stop          (!has_AC_delay_stop ? 0 : fact.get<int16_t> (Tags::AC_delay_stop)),
    delay_start            (fact.get<int16_t> (Tags::delay_start)),
    delay_stop             (fact.get<int16_t> (Tags::delay_stop)),
    has_transition_delay_start (1 == fact.count (Tags::transition_delay_start)),
    transition_delay_start (!has_transition_delay_start ? 0 : fact.get<int16_t> (Tags::transition_delay_start)),
    has_transition_delay_stop (1 == fact.count (Tags::transition_delay_stop)),
    transition_delay_stop  (!has_transition_delay_stop ? 0 : fact.get<int16_t> (Tags::transition_delay_stop)),
    ECM_rep_period         (fact.get<uint16_t> (Tags::ECM_rep_period)),
    max_streams            (fact.get<uint16_t> (Tags::max_streams)),
    min_CP_duration        (fact.get<uint16_t> (Tags::min_CP_duration)),
    lead_CW                (fact.get<uint8_t>  (Tags::lead_CW)),
    CW_per_msg             (fact.get<uint8_t>  (Tags::CW_per_msg)),
    max_comp_time          (fact.get<uint16_t> (Tags::max_comp_time))
{
}

void ts::ecmgscs::ChannelStatus::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::section_TSpkt_flag, section_TSpkt_flag);
    if (has_AC_delay_start) {
        fact.put (Tags::AC_delay_start, AC_delay_start);
    }
    if (has_AC_delay_stop) {
        fact.put (Tags::AC_delay_stop, AC_delay_stop);
    }
    fact.put (Tags::delay_start, delay_start);
    fact.put (Tags::delay_stop, delay_stop);
    if (has_transition_delay_start) {
        fact.put (Tags::transition_delay_start, transition_delay_start);
    }
    if (has_transition_delay_stop) {
        fact.put (Tags::transition_delay_stop, transition_delay_stop);
    }
    fact.put (Tags::ECM_rep_period, ECM_rep_period);
    fact.put (Tags::max_streams, max_streams);
    fact.put (Tags::min_CP_duration, min_CP_duration);
    fact.put (Tags::lead_CW, lead_CW);
    fact.put (Tags::CW_per_msg, CW_per_msg);
    fact.put (Tags::max_comp_time, max_comp_time);
}

std::string ts::ecmgscs::ChannelStatus::dump (size_t indent) const
{
    return Format ("%*schannel_status (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpDecimal (indent, "section_TSpkt_flag", section_TSpkt_flag ? 1 : 0) +
        dumpOptionalDecimal (indent, "AC_delay_start", has_AC_delay_start, AC_delay_start) +
        dumpOptionalDecimal (indent, "AC_delay_stop", has_AC_delay_stop, AC_delay_stop) +
        dumpDecimal (indent, "delay_start", delay_start) +
        dumpDecimal (indent, "delay_stop", delay_stop) +
        dumpOptionalDecimal (indent, "transition_delay_start", has_transition_delay_start, transition_delay_start) +
        dumpOptionalDecimal (indent, "transition_delay_stop", has_transition_delay_stop, transition_delay_stop) +
        dumpDecimal (indent, "ECM_rep_period", ECM_rep_period) +
        dumpDecimal (indent, "max_streams", max_streams) +
        dumpDecimal (indent, "min_CP_duration", min_CP_duration) +
        dumpDecimal (indent, "lead_CW", lead_CW) +
        dumpDecimal (indent, "CW_per_msg", CW_per_msg) +
        dumpDecimal (indent, "max_comp_time", max_comp_time);
}

    
//----------------------------------------------------------------------------
// channel_close
//----------------------------------------------------------------------------

ts::ecmgscs::ChannelClose::ChannelClose () :
    ChannelMessage (ecmgscs::Protocol::Instance()->version(), Tags::channel_close)
{
}

ts::ecmgscs::ChannelClose::ChannelClose (const tlv::MessageFactory& fact) :
    ChannelMessage (fact.protocolVersion(),
                    fact.commandTag(),
                    fact.get<uint16_t> (Tags::ECM_channel_id))
{
}

void ts::ecmgscs::ChannelClose::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
}

std::string ts::ecmgscs::ChannelClose::dump (size_t indent) const
{
    return Format ("%*schannel_close (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id);
}


//----------------------------------------------------------------------------
// channel_error
//----------------------------------------------------------------------------

ts::ecmgscs::ChannelError::ChannelError () :
    ChannelMessage (ecmgscs::Protocol::Instance()->version(), Tags::channel_error)
{
}

ts::ecmgscs::ChannelError::ChannelError (const tlv::MessageFactory& fact) :
    ChannelMessage (fact.protocolVersion(),
                    fact.commandTag(),
                    fact.get<uint16_t> (Tags::ECM_channel_id))
{
    fact.get (Tags::error_status, error_status);
    fact.get (Tags::error_information, error_information);
}

void ts::ecmgscs::ChannelError::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::error_status, error_status);
    fact.put (Tags::error_information, error_information);
}

std::string ts::ecmgscs::ChannelError::dump (size_t indent) const
{
    return Format ("%*schannel_error (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpVector (indent, "error_status", error_status) +
        dumpVector (indent, "error_information", error_information);
}


//----------------------------------------------------------------------------
// stream_setup
//----------------------------------------------------------------------------

ts::ecmgscs::StreamSetup::StreamSetup () :
    StreamMessage       (ecmgscs::Protocol::Instance()->version(), Tags::stream_setup),
    ECM_id              (0),
    nominal_CP_duration (0)
{
}

ts::ecmgscs::StreamSetup::StreamSetup (const tlv::MessageFactory& fact) :
    StreamMessage       (fact.protocolVersion(),
                         fact.commandTag(),
                         fact.get<uint16_t> (Tags::ECM_channel_id),
                         fact.get<uint16_t> (Tags::ECM_stream_id)),
    ECM_id              (fact.get<uint16_t> (Tags::ECM_id)),
    nominal_CP_duration (fact.get<uint16_t> (Tags::nominal_CP_duration))
{
}

void ts::ecmgscs::StreamSetup::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id,      channel_id);
    fact.put (Tags::ECM_stream_id,       stream_id);
    fact.put (Tags::ECM_id,              ECM_id);
    fact.put (Tags::nominal_CP_duration, nominal_CP_duration);
}

std::string ts::ecmgscs::StreamSetup::dump (size_t indent) const
{
    return Format ("%*sstream_setup (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id) +
        dumpHexa (indent, "ECM_id", ECM_id) +
        dumpDecimal (indent, "nominal_CP_duration", nominal_CP_duration);
}


//----------------------------------------------------------------------------
// stream_test
//----------------------------------------------------------------------------

ts::ecmgscs::StreamTest::StreamTest () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::stream_test)
{
}

ts::ecmgscs::StreamTest::StreamTest (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id))
{
}

void ts::ecmgscs::StreamTest::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
}

std::string ts::ecmgscs::StreamTest::dump (size_t indent) const
{
    return Format ("%*sstream_test (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id);
}


//----------------------------------------------------------------------------
// stream_status
//----------------------------------------------------------------------------

ts::ecmgscs::StreamStatus::StreamStatus () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::stream_status),
    ECM_id        (0),
    access_criteria_transfer_mode (false)
{
}

ts::ecmgscs::StreamStatus::StreamStatus (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id)),
    ECM_id        (fact.get<uint16_t> (Tags::ECM_id)),
    access_criteria_transfer_mode (fact.get<bool> (Tags::access_criteria_transfer_mode))
{
}

void ts::ecmgscs::StreamStatus::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
    fact.put (Tags::ECM_id,         ECM_id);
    fact.put (Tags::access_criteria_transfer_mode, access_criteria_transfer_mode);
}

std::string ts::ecmgscs::StreamStatus::dump (size_t indent) const
{
    return Format ("%*sstream_status (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id) +
        dumpHexa (indent, "ECM_id", ECM_id) +
        dumpDecimal (indent, "access_criteria_transfer_mode", access_criteria_transfer_mode ? 1 : 0);
}


//----------------------------------------------------------------------------
// stream_close_request
//----------------------------------------------------------------------------

ts::ecmgscs::StreamCloseRequest::StreamCloseRequest () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::stream_close_request)
{
}

ts::ecmgscs::StreamCloseRequest::StreamCloseRequest (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id))
{
}

void ts::ecmgscs::StreamCloseRequest::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
}

std::string ts::ecmgscs::StreamCloseRequest::dump (size_t indent) const
{
    return Format ("%*sstream_close_request (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id);
}


//----------------------------------------------------------------------------
// stream_close_response
//----------------------------------------------------------------------------

ts::ecmgscs::StreamCloseResponse::StreamCloseResponse () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::stream_close_response)
{
}

ts::ecmgscs::StreamCloseResponse::StreamCloseResponse (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id))
{
}

void ts::ecmgscs::StreamCloseResponse::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
}

std::string ts::ecmgscs::StreamCloseResponse::dump (size_t indent) const
{
    return Format ("%*sstream_close_response (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id);
}


//----------------------------------------------------------------------------
// stream_error
//----------------------------------------------------------------------------

ts::ecmgscs::StreamError::StreamError () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::stream_error)
{
}

ts::ecmgscs::StreamError::StreamError (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id))
{
    fact.get (Tags::error_status, error_status);
    fact.get (Tags::error_information, error_information);
}

void ts::ecmgscs::StreamError::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id, stream_id);
    fact.put (Tags::error_status, error_status);
    fact.put (Tags::error_information, error_information);
}

std::string ts::ecmgscs::StreamError::dump (size_t indent) const
{
    return Format ("%*sstream_error (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id) +
        dumpVector (indent, "error_status", error_status) +
        dumpVector (indent, "error_information", error_information);
}


//----------------------------------------------------------------------------
// CW_provision
//----------------------------------------------------------------------------

ts::ecmgscs::CWProvision::CWProvision () :
    StreamMessage       (ecmgscs::Protocol::Instance()->version(), Tags::CW_provision),
    CP_number           (0),
    has_CW_encryption   (false),
    has_CP_duration     (false),
    CP_duration         (0),
    has_access_criteria (false)
{
}

ts::ecmgscs::CWProvision::CWProvision (const tlv::MessageFactory& fact) :
    StreamMessage       (fact.protocolVersion(),
                         fact.commandTag(),
                         fact.get<uint16_t> (Tags::ECM_channel_id),
                         fact.get<uint16_t> (Tags::ECM_stream_id)),
    CP_number           (fact.get<uint16_t> (Tags::CP_number)),
    has_CW_encryption   (1 == fact.count (Tags::CW_encryption)),
    has_CP_duration     (1 == fact.count (Tags::CP_duration)),
    CP_duration         (!has_CP_duration ? 0 : fact.get<uint16_t> (Tags::CP_duration)),
    has_access_criteria (1 == fact.count (Tags::access_criteria))
{
    if (has_CW_encryption) {
        fact.get (Tags::CW_encryption, CW_encryption);
    }
    if (has_access_criteria) {
        fact.get (Tags::access_criteria, access_criteria);
    }
    std::vector<tlv::MessageFactory::Parameter> plist;
    fact.get (Tags::CP_CW_combination, plist);
    CP_CW_combination.resize (plist.size());
    for (size_t i = 0; i < plist.size (); ++i) {
        assert (plist[i].length >= 2); // already enforced in protocol def.
        CP_CW_combination[i].CP = GetUInt16 (plist[i].addr);
        CP_CW_combination[i].CW.copy (static_cast<const uint8_t*> (plist[i].addr) + 2, plist[i].length - 2);
    }
}

void ts::ecmgscs::CWProvision::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
    fact.put (Tags::CP_number,      CP_number);
    if (has_CW_encryption) {
        fact.put (Tags::CW_encryption, CW_encryption);
    }
    if (has_CP_duration) {
        fact.put (Tags::CP_duration, CP_duration);
    }
    if (has_access_criteria) {
        fact.put (Tags::access_criteria, access_criteria);
    }
    std::vector<CPCWCombination>::const_iterator it;
    for (it = CP_CW_combination.begin(); it != CP_CW_combination.end(); ++it) {
        tlv::Serializer f (fact);
        f.openTLV (Tags::CP_CW_combination);
        f.put (it->CP);
        f.put (it->CW);
        f.closeTLV ();
    }
}

std::string ts::ecmgscs::CWProvision::dump (size_t indent) const
{
    std::string dump =
        Format ("%*sCW_provision (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id) +
        dumpDecimal (indent, "CP_number", CP_number) +
        dumpOptional (indent, "CW_encryption", has_CW_encryption, CW_encryption, hexa::HEXA) +
        dumpOptionalDecimal (indent, "CP_duration", has_CP_duration, CP_duration) +
        dumpOptional (indent, "access_criteria", has_access_criteria, access_criteria, hexa::HEXA);

    for (std::vector<CPCWCombination>::const_iterator it = CP_CW_combination.begin(); it != CP_CW_combination.end(); ++it) {
        dump += dumpDecimal (indent, "CP", it->CP);
        dump += dumpOptional (indent, "CW", true, it->CW, hexa::SINGLE_LINE);
    }

    return dump;
}


//----------------------------------------------------------------------------
// ECM_response
//----------------------------------------------------------------------------

ts::ecmgscs::ECMResponse::ECMResponse () :
    StreamMessage (ecmgscs::Protocol::Instance()->version(), Tags::ECM_response),
    CP_number     (0)
{
}

ts::ecmgscs::ECMResponse::ECMResponse (const tlv::MessageFactory& fact) :
    StreamMessage (fact.protocolVersion(),
                   fact.commandTag(),
                   fact.get<uint16_t> (Tags::ECM_channel_id),
                   fact.get<uint16_t> (Tags::ECM_stream_id)),
    CP_number     (fact.get<uint16_t> (Tags::CP_number))
{
    fact.get (Tags::ECM_datagram, ECM_datagram);
}

void ts::ecmgscs::ECMResponse::serializeParameters (tlv::Serializer& fact) const
{
    fact.put (Tags::ECM_channel_id, channel_id);
    fact.put (Tags::ECM_stream_id,  stream_id);
    fact.put (Tags::CP_number,      CP_number);
    fact.put (Tags::ECM_datagram,   ECM_datagram);
}

std::string ts::ecmgscs::ECMResponse::dump (size_t indent) const
{
    return Format ("%*sECM_response (ECMG<=>SCS)\n", int (indent), "") +
        tlv::Message::dump (indent) +
        dumpHexa (indent, "ECM_channel_id", channel_id) +
        dumpHexa (indent, "ECM_stream_id", stream_id) +
        dumpDecimal (indent, "CP_number", CP_number) +
        dumpOptional (indent, "ECM_datagram", true, ECM_datagram, hexa::HEXA);
}