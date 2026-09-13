# ITCH Spec

## Protocol Options:
- SoupBinTCP
- Compressed SoupBinTCP
- MoldUDP64 (will use this because its the only option for FPGA, so standardise)

- Carries order level data for NYSE, NYSE American, NYSE Arca, and BATS listed securities and NASDAQ listed securities

- Instruments are identified by stock locate code, a low lying integer serving as an array index
- Dynamically assigned each day, starting with value of 1
- Will not change intraday
- For non-stock dependent messages, the value is 0

## MoldUDP64
- [Link](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/moldudp64.pdf)
- Lightweight protocol layer on top of UDP
- Purpose: listeners can detect and re-request missed packets
- Outbound packets only transmitted once, multiple messages may be aggregated into a single packet
- Downstream packets sent via UDP multicast
- Also sent via UDP unicast in response to "Request" message from listener (to request retransmission of desired packets)
- Big endian (most significant byte first), need not apply to the data contained within the "Message Data" fields of Message Blocks
- Individual messages can be from 0 to 64KB in length
- Once a session is terminated, no more messages can be sent, but receivers may still re-request messages from a terminated session
- A downstream packet may contain a payload of 0 or more data stream messages. Each packet consists of a **Header** and a series of **Message Blocks**

### Downstream Packet

**Header**
| Field Name      | Offset | Length | Value | Notes                                                                                                          |
|-----------------|--------|--------|-------|----------------------------------------------------------------------------------------------------------------|
| Session         | 0      | 10     | ANUM  | Indicates session number                                                                                       |
| Sequence number | 10     | 8      | NUM   | Sequence number of first message in packet, following messages implicitly numbered sequentially |
| Message count   | 18     | 2      | NUM   | Count of messages contained in packet (0x0: heartbeat, 0xFFFF: end of session)                            |

Heartbeat: typically transmitted once per second, contains next expected sequence number, for receivers to sense packet loss during times of low traffic
End of session: only sent when current session is complete, also contains the next expected sequence number

**Message Block**
| Field Name     | Offset | Length | Value | Notes                      |
|----------------|--------|--------|-------|----------------------------|
| Message length | *      | 2      | NUM   | Length of message in bytes |
| Message data   | *      | *      | ANUM  |                            |

### Request Packet
- Sent to re-request server
- Sent when the receiver detects a sequence number gap in received messages
- Response is a downstream packet unicast back to the receiver

| Field Name              | Offset | Length | Value | Notes                                           |
|-------------------------|--------|--------|-------|-------------------------------------------------|
| Session                 | 0      | 10     | ANUM  | Indicates session number                        |
| Sequence number         | 10     | 8      | NUM   | First requested sequence number                 |
| Requested message count | 18     | 2      | NUM   | Number of messages requested for retransmission |

## Data Types
- Integers: big endian, unsigned unless otherwise specified
- Alpha: ASCII, left justified, padded on right with spaces
- Prices: integer fields, with associated precision. Fixed point format, where precision defines # of decimal places (maximum: 200,000.0000, 0x77359400)
- Timestamps: nanoseconds since midnight

## Messages

### System Event Message
Purpose: signal market or data feed handler event

| Name            | Offset | Length | Value | Notes                               |
|-----------------|--------|--------|-------|-------------------------------------|
| Message Types   | 0      | 1      | 'S'   |                                     |
| Stock locate    | 1      | 2      | Int   | Always 0                            |
| Tracking number | 3      | 2      | Int   | NASDAQ internal tracking number     |
| Timestamp       | 5      | 6      | Int   | Nanoseconds since midnight          |
| Event code      | 11     | 1      | Alpha | One of 'O', 'S', 'Q', 'M', 'E', 'C' |

- 'O': Start of messages. Besides time stamp messages, this is the first message sent in any trading day
- 'S': Start of system hours. Indicates that NASDAQ is open and ready to accept orders
- 'Q': Start of market hours. Market hours orders can be executed
- 'M': End of market hours. Market hours orders can no longer be executed
- 'E': End of system hours. Can still received Broken Trade messages and Order Delete messages afterward
- 'C': End of messages. This is the last message sent in any trading day.

### Stock Related Messages

#### Stock Directory

#### Stock Trading Action

#### Reg SHO Short Sale Price Test Restricted Indicator

#### Market Participant Position

#### Market-Wide Circuit Breaker (MWCB) Messaging

#### Quoting Period Update

#### Limit Up - Limit Down (LULD) Auction Collar

#### Operational Halt

### Add Order Message

#### Add Order - No Market Participant ID (MPID) Attribution
Purpose: generated for unattributed orders accepted by NASDAQ. If want to display MPID, NASDAQ recommends using "NSDQ"

| Name                   | Offset | Length | Value               | Notes                                                |
|------------------------|--------|--------|---------------------|------------------------------------------------------|
| Message type           | 0      | 1      | 'A'                 |                                                      |
| Stock locate           | 1      | 2      | Int                 | Locate code identifying type of security             |
| Track number           | 3      | 2      | Int                 | NASDAQ internal tracking number                      |
| Timestamp              | 5      | 6      | Int                 | Nanoseconds since midnight                           |
| Order reference number | 11     | 8      | Int                 | Unique reference number assigned to order            |
| Buy / Sell indicator   | 19     | 1      | Alpha               | 'B' or 'S'                                           |
| Shares                 | 20     | 4      | Int                 | Total number of order shares being added to the book |
| Stock                  | 24     | 8      | Alpha               | Stock symbol, right padded with spaces               |
| Price                  | 32     | 4      | Price(4)            | Display price of new order                           |

#### Add Order with MPID Attribution
Same as above, but message type is 'F', and "Attribution" with offset 36, and length 4, and an Alpha value, [representing](representing) the NASDAQ market participant identifier associated with the order

### Modify Order Messages

#### Order Executed Message
Purpose: sent whenever an order on the book is executed in whole or in part.
Multiple messages on the same order are cumulative.

| Name                   | Offset | Length | Value | Notes                                                                                        |
|------------------------|--------|--------|-------|----------------------------------------------------------------------------------------------|
| Message type           | 0      | 1      | 'E'   |                                                                                              |
| Stock locate           | 1      | 2      | Int   | Type of security                                                                             |
| Tracking number        | 3      | 2      | Int   | NASDAQ internal                                                                              |
| Timestamp              | 5      | 6      | Int   |                                                                                              |
| Order reference number | 11     | 8      | Int   |                                                                                              |
| Executed shares        | 19     | 4      | Int   | Number of shares executed                                                                    |
| Match number           | 23     | 8      | Int   | NASDAQ generated day unique match number of execution. Also reference in Trade Break message |

#### Order Executed With Price Message
Purpose: sent whenever an order is executed in whole or in part at a price different from the initial price
Executions may be marked as non-printable, meaning the shares will be included into a later bulk print (e.g. look at cross executions)
Should be ignored in time and sales displays or volume calculations

Same format as above, but message type is 'C', "Printable" is an Alpha with offset 31 and length 1, being either 'N' (non-printable) or 'Y' (printable). Execution price is of format Price(4) at offset 32 and length 4.

#### Order Cancel Message
Purpose: sent when an order is modified as a result of partial cancellation (not all shares removed)o

| Name                   | Offset | Length | Value | Notes                                            |
|------------------------|--------|--------|-------|--------------------------------------------------|
| Message type           | 0      | 1      | 'X'   |                                                  |
| Stock locate           | 1      | 2      | Int   | Security type                                    |
| Tracking number        | 3      | 2      | Int   | NASDAQ internal                                  |
| Timestamp              | 5      | 6      | Int   |                                                  |
| Order reference number | 11     | 8      | Int   |                                                  |
| Cancelled shares       | 19     | 4      | Int   | Number of shares being removed from display size |

#### Order Delete Message
Purpose: sent when an order is fully cancelled. Remaining shares are no longer accessible so the order must be removed from the book

The message type is 'D'.
Same format as above but without the "cancelled shares" field.

#### Order Replace Message
Purpose: sent when an order has been cancel-replaced. Equivalent to a delete, then a new order.
The message type is 'U'.
The side, stock symbol, and attribution cannot be changed, and thus should be retained from the original order message.

| Name                            | Offset | Length | Value    | Notes                                                                        |
|---------------------------------|--------|--------|----------|------------------------------------------------------------------------------|
| Message type                    | 0      | 1      | 'U'      |                                                                              |
| Stock locate                    | 1      | 2      | Int      | Security type                                                                |
| Tracking number                 | 3      | 2      | Int      | NASDAQ internal                                                              |
| Timestamp                       | 5      | 6      | Int      |                                                                              |
| Original order reference number | 11     | 8      | Int      | Reference number of order being replaced                                     |
| New order reference number      | 19     | 8      | Int      | New reference number for this order, will be used for all subsequent updates |
| Shares                          | 27     | 4      | Int      | New total displayed quantity                                                 |
| Price                           | 31     | 4      | Price(4) | New display price                                                            |

### Trade Messages

#### Trade Message (Non-Cross)
Purpose: provide execution details for normal match events involving **non-displayable order types**, as no add order message is generated for these orders, and the order executed messages cannot be used.
Transmitted when a non-displayable order is executed in whole or in part.
They are cumulative.
They should be included in time and sales displays as well as volume and other market statistics, but they do not affect the displayable book.

| Name                   | Offset | Length | Value    | Notes                                                                                     |
|------------------------|--------|--------|----------|-------------------------------------------------------------------------------------------|
| Message type           | 0      | 1      | 'P'      |                                                                                           |
| Stock locate           | 1      | 2      | Int      | Security type                                                                             |
| Tracking number        | 3      | 2      | Int      | NASDAQ internal                                                                           |
| Timestamp              | 5      | 6      | Int      |                                                                                           |
| Order reference number | 11     | 8      | Int      | Zero / null filled bytes                                                                  |
| Buy / Sell indicator   | 19     | 1      | Alpha    | 'B' or 'S'                                                                                |
| Shares                 | 20     | 4      | Int      | Number of shares being matched                                                            |
| Stock                  | 24     | 8      | Alpha    | Stock symbol, right padded with spaces                                                    |
| Price                  | 32     | 4      | Price(4) | Match price of order                                                                      |
| Match number           | 36     | 8      | Int      | NASDAQ generated session unique match number for trade, referenced in Trade Break message |

#### Cross Trade Message
What is it: the exchange collects a batch of buy and sell orders over a period, then computes a single clearing price that maximises the matched volume, and executes all matched orders at that one price simultaneously, happening at specific, scheduled points in the trading day.
Purpose: indicates that NASDAQ has completed its cross process for a specific security. Sent out for all active issues in the system following the Opening, Closing, and EMC cross events.
For most issues, message will indicate the bulk volume associated with the cross event.
Firms should not include transactions marked as non-printable in time and sales displays or market statistic calculations.

| Name            | Offset | Length | Value    | Notes                                             |
|-----------------|--------|--------|----------|---------------------------------------------------|
| Message type    | 0      | 1      | 'Q'      |                                                   |
| Stock locate    | 1      | 2      | Int      | Security type                                     |
| Tracking number | 3      | 2      | Int      | NASDAQ internal                                   |
| Timestamp       | 5      | 6      | Int      |                                                   |
| Shares          | 11     | 8      | Int      | Number of matched shares                          |
| Stock           | 19     | 8      | Alpha    | Stock symbol, right padded with spaces            |
| Cross price     | 27     | 4      | Price(4) | Price at which cross occurred                     |
| Match number    | 31     | 8      | Int      | NASDAQ generated day unique match number          |
| Cross type      | 39     | 1      | Alpha    | 'O': NASDAQ opening cross                         |
|                 |        |        |          | 'C': NASDAQ closing cross                         |
|                 |        |        |          | 'H': Cross for IPO and halted / paused securities |

#### Broken Trade / Order Execution Message
Purpose: sent when an execution on NASDAQ is broken. (e.g. clearly erroneous pursuant to NASDAQ's clearly erroneous policy)
Once a trade is broken, it cannot be reinstated.

| Name            | Offset | Length | Value   | Notes                                                                                             |
|-----------------|--------|--------|---------|---------------------------------------------------------------------------------------------------|
| Message type    | 0      | 1      | 'B'     |                                                                                                   |
| Stock locate    | 1      | 2      | Int     | Security type                                                                                     |
| Tracking number | 3      | 2      | Int     | NASDAQ internal                                                                                   |
| Timestamp       | 5      | 6      | Integer |                                                                                                   |
| Match number    | 11     | 8      | Int     | NASDAQ match number of the execution that was broken, from previous transmitted order executed,   |
|                 |        |        |         | order executed with price, or trade message                                                       |

### Net Order Imbalance Indicator (NOII) Message

- indicative clearing price and net order imbalance (unmatched orders) on NASDAQ
- includes non-displayable as well as displayable order types
- best predictor of NASDAQ opening and closing prices available to the public

### Retail Price Improvement Indicator (RPII)

### Direct Listing with Capital Raise Price Discovery Message
