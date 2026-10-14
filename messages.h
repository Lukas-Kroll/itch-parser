//
// Created by Lukas on 01.06.2026.
//
#pragma once
#ifndef ITCH_PARSER_MESSAGES_H
#define ITCH_PARSER_MESSAGES_H
#pragma pack(push, 1)
#include <cstdint>

struct System_Event_Message
{
    char messageType; // "S" //
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    char eventCode;
};

struct Stock_Related_Message
{
    char messageType; // "R"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char marketCategory;
    char financialStatusIndicator;
    uint32_t roundLotSize;
    char roundLotsOnly;
    char issueClarification;
    uint16_t issueSubType;
    char authenticity;
    char shortSaleThresholdIndicator;
    char ipoFlag;
    char luldReferencePriceTier;
    char etpFlag;
    uint32_t etpLeverageFactor;
    char inverseIndicator;
};

struct Stock_Trading_Action
{
    char messageType; // "H"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char tradingState;
    char reserved;
    uint32_t reason;
};

struct Reg_SHO_Restriction
{
    char messageType; // "Y"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char regSHOAction;
};

struct Market_Participant_Position
{
    char messageType; // "L"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint32_t mPID;
    uint64_t stock;
    char primaryMarketMaker;
    char marketMakerMode;
    char marketParticipantState;
};

struct Market_Wide_Circuit_Breaker_Decline_Level_Message
{
    char messageType; // "V"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t level1;
    uint64_t level2;
    uint64_t level3;
};

struct Market_Wide_Circuit_Breaker_Status_Message
{
    char messageType; // "W"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    char breachedLevel;
};

struct IPO_Quoting_Period_Update
{
    char messageType; // "K"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    uint32_t ipoQuotationReleaseTime;
    char ipoQuotationReleaseQualifier;
    uint32_t ipoPrice;
};

struct Limit_Up_Limit_Down_Auction_Collar
{
    char messageType; // "J"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    uint32_t auctionCollarReferencePrice;
    uint32_t upperAuctionCollarPrice;
    uint32_t lowerAuctionCollarPrice;
    uint32_t auctionCollarExtension;
};

struct Operational_Halt
{
    char messageType; // "h"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char marketCode;
    char operationalHaltAction;
};

struct Add_Order_Message
{
    char messageType; // "A"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    char buySellIndicator;
    uint32_t shares;
    uint64_t stock;
    uint32_t price;
};

struct Add_Order_MPID_Attribution_Message
{
    char messageType; // "F"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    char buySellIndicator;
    uint32_t shares;
    uint64_t stock;
    uint32_t price;
    uint32_t attribution;
};

struct Order_Executed_Message
{
    char messageType; // "E"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
};

struct Order_Executed_With_Price_Message
{
    char messageType; // "C"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
    char printable;
    uint32_t executionPrice;
};

struct Order_Cancel_Message
{
    char messageType; // "X"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    uint32_t cancelledShares;
};

struct Order_Delete_Message
{
    char messageType; // "D"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
};

struct Order_Replace_Message
{
    char messageType; // "U"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t originalOrderReferenceNumber;
    uint64_t newOrderReferenceNumber;
    uint32_t shares;
    uint32_t price;
};

struct Trade_Message_Non_Cross
{
    char messageType; // "p"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t orderReferenceNumber;
    char buySellIndicator;
    uint32_t shares;
    uint64_t stock;
    uint32_t price;
    uint64_t matchNumber;
};

struct Trade_Message_Cross
{
    char messageType; // "Q"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t shares;
    uint64_t stock;
    uint32_t crossPrice;
    uint64_t matchNumber;
    char crossType;
};

struct Broken_Trade_Message
{
    char messageType; // "B"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t matchNumber;
};

struct Net_Order_Imbalance_Indicator_Message
{
    char messageType; // "I"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t pairedShares;
    uint64_t imbalanceShares;
    char imbalanceDirection;
    uint64_t stock;
    uint32_t farPrice;
    uint32_t nearPrice;
    uint32_t currentReferencePrice;
    char crossType;
    char priceVariationIndicator;
};

struct Retail_Price_Improvement_Indicator
{
    char messageType; // "N"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char interestFlag;
};

struct Direct_Listing_With_Capital_Raise_Price_Discovery_Message
{
    char messageType; // "O"
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint8_t timestamp[6];
    uint64_t stock;
    char openEligibilityStatus;
    uint32_t minimumAllowablePrice;
    uint32_t maximumAllowablePrice;
    uint32_t nearExecutionPrice;
    uint64_t nearExecutionTime;
    uint32_t lowerPriceRangeCollar;
    uint32_t upperPriceRangeCollar;
};

#pragma pack(pop)
#endif //ITCH_PARSER_MESSAGES_H
