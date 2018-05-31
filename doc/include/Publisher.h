#ifndef AWL_PUBLISHER_H
#define AWL_PUBLISHER_H

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


#include "DetectionStruct.h"

/** \brief Simple thread-safe class for a object that needs to inform
  * other objects of changes to its contents..
  * Just helps determine when a Publisher has an updated "publication" in store for
  * a given subscriber.
  * A Publisher keeps track of its current publication number, and also keeps track of the
  * latest issue that was consumed (Locked) by the subscriber.
  * This way, it can tell the subscriber which issue was the last consumed.
  *
  * \Notes:
  *    For a simple use of the class, a subscriber does:
  *        PublisherDerivateClass publisher();
  *        Publisher::SubscriberID subscriberID= publisher->Subscribe();  // Subscribe to a publisher
  *        // In application event loop
  *        if (publisher->HasNews(subscriberID))
  *        {
  *            if (publisher->LockNews(subscriberID));  // Informs the publisher and locks the mutex;
  *			   {
  *                ... // Process the publisher news
  *                publisher->GetIssueDerivateClassMethod(latestIssueNumber);
  *			       publisher->UnlockNews(subscriberID);  // Frees the lock on the news.
  *            }
  *        }
  *
  *    Since some publishers also have non-incremental Publication ID, a more complex subscriber could:
  *        PublisherDerivateClass publisher();
  *        Publisher::SubscriberID subscriberID= publisher->Subscribe();  // Subscribe to a publisher
  *
  *        // In application event loop
  *        if (publisher->HasNews())
  *        {
  *            Publisher::IssueID latestIssue = publisher->GetCurrentIssueID();
  *            Publisher::IssueID issueOnHand =  publisher->GetConsumedIssueID();
  *            Publisher::IssueID issueRequested =  issueOnHand;
  *            ... // Process all of the back issues
  *            do
  *			   {
  *                issueRequested++;  // Request the next issue.
  *                if (publisher->LockNews(subscriberID, issueRequested);  // Informs the publisher and locks the mutex;
  *			       {
  *                     publisher->GetIssueDerivateClassMethod(issueRequested);
  *			            publisher->UnlockNews(subscriberID);  // Frees the lock on the news.
  *				   }
  *            } while (issueRequested != lastIssue);
  *        }
  *
  *        ....
  *        // You may reuse the IssueID to request a re-issue from the publisher.
  *        publisher->GetIssueDerivateClassMethod(issueOnHandNumber);
  * \author Jean-Yves Desch�nes
  */


namespace awl {

    class Publisher {
    public:
        typedef int SubscriberID;
        typedef uint32_t IssueID;

        typedef std::shared_ptr<Publisher> Ptr;
        typedef std::vector<Publisher::Ptr> List;

        Publisher();

        Publisher::SubscriberID Subscribe();

        // Returns true if the current publication ID is differerent from the latest accessed.
        // Note that the publications are not kept in store

        bool HasNews(SubscriberID inSubscriber);

        // Locks the publisher's mutex for accessing the news.
        // and marks the current publication ID as consumed for the given subscriber
        // \return Returns true if successfully locked.  Otherwise returns false.
        //         (Lock may fail if subscriber ID is invalid).
        bool LockNews(SubscriberID inSubscriber);

        // Locks the publisher's mutex for accessing the news.
        // and marks the provided publication ID as the last consumed for the given subscriber
        // \return Returns true if successfully locked.  Otherwise returns false.
        //         (Lock may fail if subscriber ID is invalid).
        bool LockNews(SubscriberID inSubscriber, IssueID inIssueID);

        // Unlocks the publisher's mutex for accessing the news.
        void UnlockNews(SubscriberID inSubscriber);

        // Get the current publication ID for this subscriber
        IssueID GetCurrentIssueID(SubscriberID inSubscriber);

        // Get the publication ID of the latest issue that was Locked()
        IssueID GetConsumedIssueID(SubscriberID inSubscriber);

    protected:
        // Increments the publications number for all subscribers
        void PutNews();

        // Sets the latest publication number for all subscribers
        void PutNews(IssueID newPublication);

        std::mutex& GetMutex() { return (mMutex); };

        // For each subscriber, the vector contains the issue number of latest news.
        // Note: There is no guarantee from the publisher that the IssueIDs will be consecutive.
        //       The only guarantee is that consecutive publications will not have the same number for a reasonable amoount of time.
        //       (For example, there may be wraparound of the IssueIDs)
        std::vector<IssueID> currentPublications;

        // For each subscriber, the vector contains the issue number last Locked() news.
        std::vector<IssueID> consumedPublications;

        std::mutex mMutex;

    }; // Publisher

} // namespace awl
#endif  // AWL_PUBLISHER_H
