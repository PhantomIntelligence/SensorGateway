/* Publisher.cpp */
/*
	Copyright 2014, 2015 Phantom Intelligence Inc.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/
#include <iostream>

#include "SensorCommunication/Publisher.h"

using awl::Publisher;

Publisher::Publisher() {
    currentPublications.clear();
    consumedPublications.clear();
}

Publisher::SubscriberID Publisher::Subscribe() {
    IssueID IssueID = 0;
    currentPublications.push_back(IssueID);
    consumedPublications.push_back(IssueID);

    // The subscriberID is actually the index in the subscriber array
    return ((SubscriberID) currentPublications.size() - 1);
}

bool Publisher::HasNews(SubscriberID inSubscriber) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return (false);
    } else {
        return (currentPublications.at(inSubscriber) != consumedPublications.at(inSubscriber));
    }
}

bool Publisher::LockNews(SubscriberID inSubscriber) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return (false);
    }

    consumedPublications.at(inSubscriber) = currentPublications.at(inSubscriber);
    mMutex.lock();
    return (true);
}

bool Publisher::LockNews(SubscriberID inSubscriber, IssueID inIssueID) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return (false);
    }

    consumedPublications.at(inSubscriber) = inIssueID;
    mMutex.lock();
    return (true);
}

void Publisher::UnlockNews(SubscriberID inSubscriber) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return;
    }

    mMutex.unlock();
}


void Publisher::PutNews() {
    int subscriberQty = currentPublications.size();
    for (int i = 0; i < subscriberQty; i++) {
        mMutex.lock();
        IssueID issueID = currentPublications.at(i);
        issueID++;
        currentPublications.at(i) = issueID;
        mMutex.unlock();
    }
}

void Publisher::PutNews(IssueID inIssueID) {
    int subscriberQty = currentPublications.size();
    for (int i = 0; i < subscriberQty; i++) {
        mMutex.lock();
        currentPublications.at(i) = inIssueID;
        mMutex.unlock();
    }
}

Publisher::IssueID Publisher::GetCurrentIssueID(SubscriberID inSubscriber) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return (0);
    }

    return (currentPublications.at(inSubscriber));
}

Publisher::IssueID Publisher::GetConsumedIssueID(SubscriberID inSubscriber) {
    if (inSubscriber < 0 || inSubscriber >= (int) currentPublications.size()) {
        return (0);
    }

    return (consumedPublications.at(inSubscriber));
}
