<?php
/**
 * @file
 * Provides the back end support for a javascript API for paragraphs widgets.
 */

/**
 * Attaches the back end paragraphs Ajax API behavior to a field instance.
 *
 * @param string $wrapper_id
 *   The wrapper id for the form element.
 * @param array &$element
 *   The form element to attach to.
 * @param array $context
 *   The context argument provided by the hook_field_widget_form_alter
 *   implementation.
 */
function paragraphs_extra_ajax_api_attach($wrapper_id, &$element, $context) {
  $instance_id = _paragraphs_extra_ajax_api_instance_id($context);

  paragraphs_extra_ajax_api_attach_bundle_settings($element, $context);
  paragraphs_extra_ajax_api_attach_field_settings($element, $context);
  $field_name = $context['field']['field_name'];

  // Rewrite the prefix with more rich information about the field.
  $element['#prefix'] = '<div
    id="' . $wrapper_id . '"
    class="' . _paragraphs_extra_class('ajax-api', 'field-instance') . '"' .
    _paragraphs_extra_data('ajax-api', 'field-name') . '="' . $field_name . '"' .
    _paragraphs_extra_data('ajax-api', 'instance-id') . '="' . $instance_id . '"
  >';

  // Iterate over the paragraph instances to attach metadata that the
  // front end API will use to talk to the form API.
  foreach ($element as $delta => &$item) {
    if (is_numeric($delta)) {
      $item['#prefix'] = '<div
        class="' . _paragraphs_extra_class('ajax-api', 'paragraphs-item') . '" ' .
        _paragraphs_extra_data('ajax-api', 'owner') . '="' . $instance_id . '" ' .
        _paragraphs_extra_data('ajax-api', 'delta') . '="' . $item['#delta'] . '" ' .
        _paragraphs_extra_data('ajax-api', 'bundle') . '="' . $item['#bundle'] . '"' .
      '>';
      $item['#suffix'] = '</div>';
    }
  }

  // Attach a button to trigger an ajax command to be sent.
  $element['add_more']['paragraphs_extra']['ajax_api']['command_trigger'] = array(
    '#type' => 'submit',
    '#value' => t('Execute Paragraphs API'),
    '#weight' => 9999,
    '#limit_validation_errors' => array(),
    '#submit' => array('paragraphs_extra_ajax_api_command_submit'),
    '#ajax' => array(
      'callback' => 'paragraphs_extra_ajax_api_command_ajax',
      'wrapper' => $wrapper_id,
      'effect' => 'fade',
    ),
    '#attributes' => array(
      'class' => array(
        'element-invisible',
        _paragraphs_extra_class('ajax-api', 'command-trigger'),
      ),
      _paragraphs_extra_data('ajax-api', 'owner') => $instance_id,
    ),
  );

  // Attach a field for holding information passed over AJAX.
  $element['add_more']['paragraphs_extra']['ajax_api']['command_data'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'class' => array(
        _paragraphs_extra_class('ajax-api', 'command-data'),
      ),
      _paragraphs_extra_data('ajax-api', 'owner') => $instance_id,
    ),
  );
}

/**
 * Applies ajax effects after an AJAX API triggered form rebuild.
 *
 * @param array $form
 *   The affected form object.
 * @param array &$form_state
 *   The updated form state.
 *
 * @return array
 *   The rebuilt widget form render array.
 */
function paragraphs_extra_ajax_api_handle_command_ajax($form, &$form_state) {
  $button = $form_state['triggering_element'];

  // Go one level up in the form, to the widgets container.
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -4));
  $field_name = $element['#field_name'];
  $langcode = $element['#language'];
  $parents = $element['#field_parents'];

  $field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);

  // Add a DIV around the deltas receiving the Ajax effect.
  $field = $field_state['field'];
  foreach ($field_state['paragraphs_extra_ajax_api_result'] as $item) {
    $delta = $item['delta'];
    $element[$delta]['#prefix'] = '<div class="ajax-new-content">' . (isset($element[$delta]['#prefix']) ? $element[$delta]['#prefix'] : '');
    $element[$delta]['#suffix'] = (isset($element[$delta]['#suffix']) ? $element[$delta]['#suffix'] : '') . '</div>';
  }

  // Let other modules alter the added paragraphs before they are rendered.
  $context = $field_state['paragraphs_extra_ajax_api_result'];
  drupal_alter('paragraphs_extra_ajax_api_result', $element, $context);

  return $element;
}

/**
 * Handles AJAX API commands from the client side.
 *
 * @param array $form
 *   The affected form object.
 * @param array &$form_state
 *   The updated form state.
 */
function paragraphs_extra_ajax_api_handle_command_submit($form, &$form_state) {
  $button = $form_state['triggering_element'];

  // Go one level up in the form, to the widgets container.
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -4));
  $field_name = $element['#field_name'];
  $langcode = $element['#language'];
  $parents = $element['#field_parents'];
  $id_prefix = implode('-', array_merge($parents, array($field_name)));

  // Set up an execution state object.
  $state = new stdClass();
  $state->delta = 0;
  $state->sorted = array();
  $state->element = $element;
  $state->inputs = drupal_array_get_nested_value($form_state['input'], array_slice($button['#array_parents'], 0, -4));
  $state->command = drupal_json_decode($state->inputs['add_more']['paragraphs_extra']['ajax_api']['command_data']);
  $state->langcode = $langcode;
  $state->field_name = $field_name;
  $state->form_state = $form_state;
  $state->field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);

  // Execute the command.
  paragraphs_extra_ajax_api_execute_command($state);

  // Commit the changes to the field and form states, and instruct the form API
  // to rebuild the form.
  field_form_set_state($parents, $field_name, $langcode, $form_state, $state->field_state);
  drupal_array_set_nested_value($form_state['input'], array_slice($button['#array_parents'], 0, -4), $state->inputs);
  $form_state['rebuild'] = TRUE;
}

/**
 * Executes an AJAX command using a state object.
 *
 * @param object $state
 *   An object representing the current rebuild state.
 */
function paragraphs_extra_ajax_api_execute_command($state) {

  // All items that occur up to the command delta should remain as is.
  paragraphs_extra_ajax_api_copy_paragraphs($state, 0, $state->command['delta'] - 1);

  // Insert any items that should come before the command delta.
  paragraphs_extra_ajax_api_insert_new_paragraphs($state, 'insertBefore');

  // Update the command delta item.
  paragraphs_extra_ajax_api_update_paragraph($state);

  // Insert any items that should come after the command delta.
  paragraphs_extra_ajax_api_insert_new_paragraphs($state, 'insertAfter');

  // All items after the command delta should be left as is.
  paragraphs_extra_ajax_api_copy_paragraphs($state, $state->command['delta'] + 1, $state->element['#max_delta']);

  // Attach a form state value to tell the ajax processor about the command
  // result.
  $state->field_state['paragraphs_extra_ajax_api_result'] = array();
  foreach ($state->sorted as $delta => $item) {
    $state->field_state['entity'][$delta] = $item['entity'];
    $state->inputs[$delta] = $item['input'];
    $state->inputs[$delta]['_weight'] = $delta;
    if (isset($item['command'])) {
      $state->field_state['paragraphs_extra_ajax_api_result'][] = array(
        'delta' => $delta,
        'command' => $item['command'],
      );
    }
  }
}

/**
 * Gets the next available delta in the array of rebuilt items.
 *
 * @param object $state
 *   The current rebuild state.
 *
 * @return int
 *   The next available delta in the rebuild array.
 */
function paragraphs_extra_ajax_api_next_delta($state) {
  while (isset($state->sorted[$state->delta])) {
    $state->delta++;
  }
  return $state->delta;
}

/**
 * Applies an insert command to the paragraph form.
 *
 * @param object $state
 *   The current rebuild state.
 * @param string $insert
 *   The command array index key to pull the new paragraph information from.
 */
function paragraphs_extra_ajax_api_insert_new_paragraphs($state, $insert) {
  $new_paragraphs = $state->command[$insert];
  $state->field_state['items_count'] += count($new_paragraphs);
  foreach ($new_paragraphs as $new_paragraph) {
    if (isset($new_paragraph['bundle'])) {
      $new_delta = paragraphs_extra_ajax_api_next_delta($state);
      $state->sorted[$new_delta] = array(
        'entity' => paragraphs_extra_ajax_api_create_paragraph_item($new_paragraph['bundle'], $state->field_name),
        'input' => array(),
        'command' => $insert,
      );
      if (isset($new_paragraph['values'])) {
        paragraphs_extra_ajax_api_apply_values($state, $new_delta, $new_paragraph['values']);
      }
    }
  }
}

/**
 * Creates a new paragraph item entity for the widget form.
 *
 * @param string $bundle
 *   The paragraph item bundle to create.
 * @param string $field_name
 *   The name of the field the item belongs to.
 *
 * @return object
 *   A newly crated paragraphs_item entity.
 */
function paragraphs_extra_ajax_api_create_paragraph_item($bundle, $field_name) {
  $paragraph_item = entity_create('paragraphs_item', array('bundle' => $bundle, 'field_name' => $field_name));
  $paragraph_item->being_edited = TRUE;
  return $paragraph_item;
}

/**
 * Copies a paragraph from the current for to the newly rebuilt array.
 *
 * @param object $state
 *   The current rebuild state.
 * @param int $from
 *   The index to start copying from.
 * @param int $to
 *   The index to stop copying from.
 */
function paragraphs_extra_ajax_api_copy_paragraphs($state, $from, $to) {
  for ($i = $from; $i <= $to; $i++) {
    $delta = paragraphs_extra_ajax_api_next_delta($state);
    $state->sorted[$delta] = array(
      'entity' => $state->field_state['entity'][$i],
      'input' => isset($state->inputs[$i]) ? $state->inputs[$i] : array(),
    );
  }
}

/**
 * Copies over the delta being operated on to the rebuild form.
 *
 * This function can also replace the existing paragraphs item value with a new
 * set of values if the replace option is set on the form state.
 *
 * @param object $state
 *   The current rebuild state.
 */
function paragraphs_extra_ajax_api_update_paragraph($state) {
  $replace = isset($state->command['replace']) ? $state->command['replace'] : array();

  if ($replace || isset($state->field_state['entity'][$state->command['delta']])) {
    $replace_delta = paragraphs_extra_ajax_api_next_delta($state);

    $state->sorted[$replace_delta] = array(
      'entity' => $state->field_state['entity'][$state->command['delta']],
      'input' => array(),
    );

    if ($replace) {
      if (isset($replace['values'])) {
        paragraphs_extra_ajax_api_apply_values($state, $replace_delta, $replace['values']);
        $state->sorted[$replace_delta]['command'] = 'replace';
      }
    }
  }
}

/**
 * Applies an array of field values sent via an AJAX command to a state object.
 *
 * @param object $state
 *   The state object to apply the values to.
 * @param int $delta
 *   The delta to apply the values to.
 * @param array $values
 *   The values to be applied.
 */
function paragraphs_extra_ajax_api_apply_values($state, $delta, $values) {
  foreach ($values as $field_name => $field_deltas) {
    foreach ($field_deltas as $field_delta => $subvalues) {
      foreach ($subvalues as $value_name => $value) {
        $state->sorted[$delta]['input'][$field_name][$state->langcode][$field_delta][$value_name] = $value;
      }
    }
  }
}

/**
 * Exposes the paragraphs bundle settings for a field instance to the AJAX API.
 *
 * @param array &$element
 *   The form element to attach the javascript variable to.
 * @param array $context
 *   The context array for the provided form element.
 */
function paragraphs_extra_ajax_api_attach_bundle_settings(&$element, $context) {
  // Convert the default paragraph bundle settings object into an array  and
  // add a description so we can serialize it into a Drupal.settings object
  // to provide to javascript-land.
  $available_bundles = paragraphs_bundle_load();
  foreach ($available_bundles as $bundle => $info) {
    $settings = unserialize(variable_get('paragraphs_split_bundle_settings_' . $bundle, serialize(array('ckeditor_description' => ''))));
    if (isset($settings['ckeditor_description'])) {
      $available_bundles[$bundle] = (array) $available_bundles[$bundle] + $settings;
    }
    else {
      $available_bundles[$bundle] = (array) $available_bundles[$bundle] + array('ckeditor_description' => '');
    }
  }

  // Most of this code is stolen directly from the paragraphs module's field
  // widget implementation.
  //
  // @see paragraphs.field_widget.inc
  $select_bundles = array();
  $select_bundles_weighted = array();
  $explicitly_enabled = FALSE;
  foreach ($context['instance']['settings']['allowed_bundles'] as $allowed_bundle_key => $allowed_bundle_value) {
    if ($allowed_bundle_key === $allowed_bundle_value && isset($available_bundles[$allowed_bundle_key])) {
      $select_bundles[$available_bundles[$allowed_bundle_key]['bundle']] = $available_bundles[$allowed_bundle_key];

      // If an item has been explicitly selected, raise our flag.
      $explicitly_enabled = TRUE;
    }
    elseif (isset($available_bundles[$allowed_bundle_key]['bundle'])) {
      $select_bundles_weighted[$available_bundles[$allowed_bundle_key]['bundle']] = $available_bundles[$allowed_bundle_key];
    }
  }

  // If no bundle has been explicitly selected, give access to all of them.
  if (!$explicitly_enabled) {
    $select_bundles = $select_bundles_weighted;
    foreach ($available_bundles as $bundle) {
      if (!isset($select_bundles[$bundle['bundle']])) {
        $select_bundles[$bundle['bundle']] = $bundle;
      }
    }
  }
  $removed_a_bundle = FALSE;

  $weight = 0;
  $entity_shell = new stdClass();
  $entity_shell->is_new = TRUE;
  foreach ($select_bundles as $machine_name => $bundle) {
    $select_bundles[$machine_name] = $bundle + array(
      'weight' => $weight,
    );

    $entity_shell->bundle = $machine_name;
    if (!entity_access('create', 'paragraphs_item', $entity_shell)) {
      unset($select_bundles[$machine_name]);
      $removed_a_bundle = TRUE;
    }
    elseif (isset($context['instance']['settings']['allowed_bundles']['bundle_weights'][$machine_name])) {
      $select_bundles[$machine_name] = $context['instance']['settings']['allowed_bundles']['bundle_weights'][$machine_name];
    }
    ++$weight;
  }

  $instance_id = _paragraphs_extra_ajax_api_instance_id($context);
  $bundles[$instance_id] = $select_bundles;

  // Attach the javascript interface for the API.
  $element['#attached']['js'][] = drupal_get_path('module', 'paragraphs_extra') . '/js/Api.js';
  $element['#attached']['js'][] = array(
    'data' => array(
      'ParagraphsExtra' => array(
        'Api' => array(
          'bundles' => $bundles,
        ),
      ),
    ),
    'type' => 'setting',
  );
}

/**
 * Exposes the settings for a field  field instance to the AJAX API.
 *
 * @param array &$element
 *   The form element to attach the settings to.
 * @param array $context
 *   The context array for the form element.
 */
function paragraphs_extra_ajax_api_attach_field_settings(&$element, $context) {
  if (isset($context['instance']) && isset($context['instance']['settings'])) {
    $field_name = $context['field']['field_name'];
    $host_type = $context['instance']['entity_type'];
    $host_bundle = $context['instance']['bundle'];
    $instance_id = md5($field_name . ':' . $host_type . ':' . $host_bundle);
    $element['#attached']['js'][] = array(
      'data' => array(
        'ParagraphsExtra' => array(
          'Api' => array(
            'instance_settings' => array(
              $instance_id => $context['instance']['settings'],
            ),
          ),
        ),
      ),
      'type' => 'setting',
    );
  }
}

/**
 * Applies transition effects to newly added paragraph items.
 *
 * @param array $form
 *   The form being updated.
 * @param array &$form_state
 *   The form state for the form being updated.
 *
 * @return array
 *   The rebuilt paragraphs field widget render array.
 */
function paragraphs_extra_ajax_api_command_ajax($form, &$form_state) {
  paragraphs_extra_include('ajax_api');
  return paragraphs_extra_ajax_api_handle_command_ajax($form, $form_state);
}

/**
 * Provides a unique key for a field instance.
 *
 * @param array $context
 *   The context array for the form widget.
 */
function _paragraphs_extra_ajax_api_instance_id($context) {
  $field_name = $context['field']['field_name'];
  $host_type = $context['instance']['entity_type'];
  $host_bundle = $context['instance']['bundle'];
  return md5($field_name . ':' . $host_type . ':' . $host_bundle);
}
