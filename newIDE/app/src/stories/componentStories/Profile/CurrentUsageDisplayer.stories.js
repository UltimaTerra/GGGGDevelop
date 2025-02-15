// @flow
import * as React from 'react';
import { action } from '@storybook/addon-actions';
import muiDecorator from '../../ThemeDecorator';
import paperDecorator from '../../PaperDecorator';
import CurrentUsageDisplayer from '../../../Profile/CurrentUsageDisplayer';
import subscriptionSuggestionDecorator from '../../SubscriptionSuggestionDecorator';
import {
  limitsReached,
  noSubscription,
  silverSubscriptionWithExpiredRedemptionCode,
  silverSubscriptionWithRedemptionCode,
  subscriptionForIndieUser,
} from '../../../fixtures/GDevelopServicesTestData';

export default {
  title: 'Profile/CurrentUsageDisplayer',
  component: CurrentUsageDisplayer,
  decorators: [subscriptionSuggestionDecorator, paperDecorator, muiDecorator],
};

export const Default = () => (
  <CurrentUsageDisplayer
    subscription={subscriptionForIndieUser}
    currentUsage={{
      current: 2,
      max: 10,
      limitReached: false,
    }}
    onChangeSubscription={action('on change subscription callback')}
  />
);

export const With1BuildRemaining = () => (
  <CurrentUsageDisplayer
    subscription={subscriptionForIndieUser}
    currentUsage={{
      current: 4,
      max: 5,
      limitReached: false,
    }}
    onChangeSubscription={action('on change subscription callback')}
  />
);

export const WithRedemptionCode = () => (
  <CurrentUsageDisplayer
    subscription={silverSubscriptionWithRedemptionCode}
    currentUsage={{
      current: 2,
      max: 10,
      limitReached: false,
    }}
    onChangeSubscription={action('on change subscription callback')}
  />
);

export const WithExpiredRedemptionCode = () => (
  <CurrentUsageDisplayer
    subscription={silverSubscriptionWithExpiredRedemptionCode}
    currentUsage={{
      current: 2,
      max: 10,
      limitReached: false,
    }}
    onChangeSubscription={action('on change subscription callback')}
  />
);

export const LimitReached = () => (
  <CurrentUsageDisplayer
    subscription={subscriptionForIndieUser}
    currentUsage={limitsReached.limits['cordova-build']}
    onChangeSubscription={action('on change subscription callback')}
  />
);

export const LimitsReachedWithoutSubscription = () => (
  <CurrentUsageDisplayer
    subscription={noSubscription}
    currentUsage={limitsReached.limits['cordova-build']}
    onChangeSubscription={action('on change subscription callback')}
  />
);
