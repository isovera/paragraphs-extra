<?php

function paragraphs_extra_widget_attach($wrapper_id, &$element, &$form_state, $context) {
  $settings = _paragraphs_extra_widget_instance_settings($context['instance']);

  // Don't continue if the widgets behavior is not enabled.
  if (!$settings['enabled']) {
    return;
  }

  // Attach additional resources for the widget API.
  $element['#attached']['js'][] = drupal_get_path('module', 'paragraphs_extra') . '/js/Widget.js';
  $element['#attached']['css'][] = drupal_get_path('module', 'paragraphs_ckeditor') . '/css/paragraphs_ckeditor.css';

  // Add a form validation function for updating the field widget state based on
  // the selected widget and override the default paragraphs theme function with
  // a custom version that will handle widget rendering.
  $element['#element_validate'][] = 'paragraphs_extra_widget_validate';
  $element['#theme'] = 'paragraphs_extra_field_multiple_value_form';

  // Reset the default form validator, each widget will be responsible for its
  // own validation.
  foreach ($element as $delta => $item) {
    if (is_numeric($delta)) {
      unset($element[$delta]['#element_validate']);
    }
  }

  // Attach the widget settings so the theme function can have a peek at what
  // widgets it needs to render the navigation menu for. 
  $element['#paragraphs_extra_widget_settings'] = $settings;

  // Get the current entity values associated with this field.
  $parents = $context['form']['#parents'];
  $field_name = $element['#field_name'];
  $field_state = field_form_get_state($parents, $field_name, $context['langcode'], $form_state);
  $entities = isset($field_state['entity']) ? $field_state['entity'] : array();

  // Attach Paragraphs Extra widgets.
  $widgets = \Drupal\paragraphs_extra\Widget\Api::widgetInstances($settings['widgets'], $element, $form_state, $context);
  $active = \Drupal\paragraphs_extra\Widget\Api::getWidget($settings['widgets'], $element, $form_state, $context);
  foreach ($widgets as $name => $instance) {
    // Each enabled widget needs to have at least its root form element
    // created even if it is not the active widget to streamline form state
    // processing. Otherwise we need to perform additional checks to make sure
    // the widget exists and is enabled.
    $instance->init($element);
    $widget_element = array('#access' => FALSE);

    // The active widget gets to attach its element to the field widget
    // element.
    if ($instance->getType() == $active->getType()) {
      $widget_element['#access'] = TRUE;
      $instance->element($widget_element, $entities);
    }

    // Append the newly created form element in a predictable place.
    $element['add_more']['paragraphs_extra']['widget']['widgets'][$instance->getType()] = $widget_element;
  }

  // Attach fields for handling ajax switching between widgets.
  $element['add_more']['paragraphs_extra']['widget']['active'] = array(
    'value' => array(
      '#type' => 'hidden',
      '#value' => $active->getType(),
      '#attributes' => array('class' => array('paragraphs-extra-widget-value')),
    ),
    'switch' => array(
      '#type' => 'submit',
      '#attributes' => array('class' => array('paragraphs-extra-widget-switch', 'element-invisible')),
      '#value' => 'Switch Widget',
      '#limit_validation_errors' => array(),
      '#submit' => array('paragraphs_extra_widget_switch_submit'),
      '#ajax' => array(
        'callback' => 'paragraphs_extra_widget_switch_ajax',
        'wrapper' => $wrapper_id,
        'effect' => 'fade',
      ),
    ),
  );
}

function paragraphs_extra_widget_validate_hander(&$element, &$form_state, $context) {
  if (empty($element['#disabled'])) {
    // Update the overall field widget state based on the current widget's
    // submitted values.
    $settings = $element['#paragraphs_extra_widget_settings'];
    $last_active_widget = \Drupal\paragraphs_extra\Widget\Api::getCurrentWidget($settings['widgets'], $element, $form_state);
    if ($last_active_widget) {
      // Validate submited values from last active widget.
      $widget_element = $last_active_widget->getElement();
      $widget_values = $last_active_widget->getValues();
      $last_active_widget->validate($widget_element, $widget_values);

      if (!form_get_errors()) {
        // Update the form state values based on the submitted widget values.
        $state = new \Drupal\paragraphs_extra\Widget\State($element, $form_state);
        $last_active_widget->submit($state, $widget_element, $widget_values);
        $state->commit();

        // Based on the updated state, initialize the element values for the newly
        // selected widget.
        $new_active_widget = \Drupal\paragraphs_extra\Widget\Api::getWidget($settings['widgets'], $element, $form_state);
        if ($new_active_widget) {
          $values = $new_active_widget->setState($state);
          $new_active_widget->setValues($values);
        }
      }
    }
  }
  else {
    $state = new \Drupal\paragraphs_extra\Widget\State($element, $form_state);
    $state->commit();
  }
}

function paragraphs_extra_widget_switch_submit_handler($form, &$form_state) {
  // We will need to rebuild the entire form here to allow updating the form
  // state and showing the newly selected widget.
  $form_state['rebuild'] = TRUE;
}

function paragraphs_extra_widget_switch_ajax_handler($form, &$form_state) {
  // Return the entire rebuilt field widget.
  $button = $form_state['triggering_element'];
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -5));
  return $element;
}

function paragraphs_extra_widget_attach_instance_settings(&$form, &$form_state) {
  $container_id = drupal_html_id('paragraphs_extra_settings_list_container');
  $wrapper_id = _paragraphs_extra_id('widget', 'options');
  $settings = _paragraphs_extra_widget_instance_settings($form_state['build_info']['args'][0]);
  $form['instance']['settings']['paragraphs_extra']['behaviors']['widget'] = array(
    '#weight' => 10,
    '#type' => 'fieldset',
    '#title' => t('Widgets'),
    '#description' => t('Paragraphs Extra widgets provide users with multiple ways to build paragraphs content.  Select which widgets you wish to enable.'),
    '#attributes' => array(
      'id' => array($wrapper_id),
    ),
    'enabled' => array(
      '#type' => 'checkbox',
      '#title' => t('Enable Paragraphs Extra Widgets'),
      '#description' => t('Determines whether to enable Paragraphs Extra Widgets for this field.'),
      '#default_value' => $settings['enabled'],
      '#attributes' => array(
        'class' => array('widgets-enabled'),
      ),
    ),
    'widgets' => array(
      '#type' => 'container',
      'list' => array(
        '#tree' => TRUE,
        '#title' => t('Enabled Widgets'),
        '#theme' => 'paragraphs_extra_widget_settings_list',
      ),
      'settings' => array(
      ),
      '#states' => array(
        'visible' => array(
          '#' . $wrapper_id . ' .widgets-enabled'  => array('checked' => TRUE),
        ),
      ),
    ),
  );

  $extra_info = \Drupal\paragraphs_extra\Widget\Api::widgets();
  foreach ($settings['widgets']['list'] as $name => $info) {
    $checkbox_id = drupal_html_id('paragraphs-extra-widget-enabled');
    $form['instance']['settings']['paragraphs_extra']['behaviors']['widget']['widgets']['list'][$name] = array(
      'name' => array(
        '#markup' => check_plain(isset($extra_info[$name]['title']) ? $extra_info[$name]['title'] : $info['title']),
      ),
      'enabled' => array(
        '#type' => 'checkbox',
        '#default_value' => $info['enabled'],
        '#attributes' => array(
          'id' => $checkbox_id,
        ),
      ),
      'title' => array(
        '#type' => 'textfield',
        '#default_value' => $info['title'],
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#title_display' => 'invisible',
        '#default_value' => $info['weight'],
      ),
    );

    if (isset($extra_info[$name]['settings callback'])) {
      $form['instance']['settings']['paragraphs_extra']['behaviors']['widget']['widgets']['settings'][$name] = array(
        '#type' => 'fieldset',
        '#title' => $extra_info[$name]['title'],
        '#description' => t('Settings for the @name widget.', array('@name' => $extra_info[$name]['title'])),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
        '#states' => array(
          'visible' => array(
            '#' . $checkbox_id => array('checked' => TRUE),
          ),
        ),
      );

      $callback = $extra_info[$name]['settings callback'];
      if (function_exists($callback)) {
        if (isset($extra_info[$name]['file'])) {
          $file = $extra_info[$name]['file'];
          if (isset($extra_info[$name]['file path'])) {
            $file = $extra_info[$name]['file_path'] . '/' . $file;
          }
          require_once $file;
        }

        $element = &$form['instance']['settings']['paragraphs_extra']['behaviors']['widget']['widgets']['settings'][$name];
        $callback($element, $form_state, $settings['widgets']['settings'][$name]);
      }
    }
  }
}

function _paragraphs_extra_widget_instance_settings($instance) {
  $settings = _paragraphs_extra_instance_settings($instance, 'widget', array(
    'enabled' => FALSE,
    'list' => array(
      'widgets' => array(),
      'settings' => array(),
    ),
  ));
  $widgets = \Drupal\paragraphs_extra\Widget\Api::widgets();

  foreach ($widgets as $name => $info) {
    $default = array(
      'enabled' => TRUE,
      'title' => $info['title'],
      'weight' => 999,
    );

    if (isset($settings['widgets']['list'][$name])) {
      $settings['widgets']['list'][$name] += $default;
    }
    else {
      $settings['widgets']['list'][$name] = $default;
    }

    if (!isset($settings['widgets']['settings'][$name])) {
      $settings['widgets']['settings'][$name] = array();
    }
  }

  uasort($settings['widgets']['list'], 'drupal_sort_weight');

  return $settings;
}
