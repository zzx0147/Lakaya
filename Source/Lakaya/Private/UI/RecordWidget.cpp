// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RecordWidget.h"
#include "Developer/TraceAnalysis/Private/Store/AsioSocket.h"
#include "ThirdParty/asio/1.12.2/asio/ip/tcp.hpp"
#include "ThirdParty/asio/1.12.2/asio/io_context.hpp"

void URecordWidget::NativeConstruct()
{
	Super::NativeConstruct();

	asio::io_context mycontext;
	
	auto Temp = new UE::Trace::FAsioSocket(asio::ip::tcp::socket(mycontext));

	
}
