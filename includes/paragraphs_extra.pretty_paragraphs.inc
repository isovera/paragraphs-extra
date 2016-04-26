<?php
/**
 * @file
 * Provides UX enhancements for the paragraphs module.
 */

/**
 * Attaches 'Pretty Paragraphs' UX behaviors to the paragraphs field widget.
 *
 * @param string $wrapper_id
 *   The form id of the field container.
 * @param array &$element
 *   A reference to the form element to attach to.
 * @param array &$form_state
 *   The form api form state for the widget's form.
 */
function paragraphs_extra_pretty_paragraphs_attach($wrapper_id, &$element, &$form_state, $context) {
  $element['#attached']['js'][] = drupal_get_path('module', 'paragraphs_extra') . '/js/PrettyParagraphs.js';
  $element['#attached']['css'][] = drupal_get_path('module', 'paragraphs_extra') . '/css/pretty_paragraphs.css';

  // Get a list of features to enable based on the field configuration.
  $settings = _paragraphs_extra_pretty_paragraphs_instance_settings($element['#instance']);

  $element['add_more']['paragraphs_extra']['pretty_paragraphs'] = array(
    '#tree' => TRUE,
  );

  // Add view mode selection toolbar.
  if ($settings['preview_modes']) {
    $viewmode = paragraphs_extra_pretty_paragraphs_attach_viewmode_toolbar($wrapper_id, $element, $form_state);
  }
  else {
    $viewmode = 'paragraphs_editor_preview';
  }

  foreach ($element as $delta => $item) {
    if (is_numeric($delta)) {
      $element[$delta]['paragraphs_extra']['pretty_paragraphs'] = array(
        '#tree' => TRUE,
      );

      // Decorate built-in paragraphs buttons so we can find them in javascript.
      $element[$delta]['actions']['remove_button']['#attributes']['class'][]
        = _paragraphs_extra_class('pretty-paragraphs', 'delete-button');
      $element[$delta]['actions']['confirm_delete_button']['#attributes']['class'][]
        = _paragraphs_extra_class('pretty-paragraphs', 'confirm-delete-button');

      // Add the beautified admin buttons for the paragraph.
      if ($settings['click_to_edit']) {
        paragraphs_extra_pretty_paragraphs_attach_edit_toolbar($element[$delta], $viewmode);
      }

      // Add the insert button between paragraph items.
      if ($settings['insert_between']) {
        paragraphs_extra_pretty_paragraphs_attach_insert_toolbar($element[$delta], $context);
      }
    }
  }

  // Switches to a modal-based "Add Paragraph" bundle selection UI.
  if ($settings['modal_select']) {
    paragraphs_extra_pretty_paragraphs_attach_add_button($element, $context);
  }
}

/**
 * Creates a toolbar with an 'edit' and 'delete' button.
 *
 * @param array &$element
 *   The paragraph item for mto attach to.
 * @param string $viewmode
 *   The view mode to render previews in.
 */
function paragraphs_extra_pretty_paragraphs_attach_edit_toolbar(&$element, $viewmode) {
  $element['paragraph_bundle_title']['#attributes']['class'][] = 'element-invisible';

  // Attach a rendered preview of the paragraph element.
  paragraphs_extra_pretty_paragraphs_attach_preview($element, $viewmode);

  $icon_class = _paragraphs_extra_class('pretty-paragraphs', 'icon');
  $button_class = _paragraphs_extra_class('pretty-paragraphs', 'button');
  $mobile_only = _paragraphs_extra_class('mobile-only');

  // Load SVG icons for the admin interface.
  $icon_pack = drupal_get_path('module', 'paragraphs_extra') . '/images/pretty_paragraphs_icons.svg';

  $switch_icons = '<svg class="' . $icon_class . ' ' . $icon_class . '--edit">';
  $switch_icons .= '<use xlink:href="/' . $icon_pack . '#edit"></use>';
  $switch_icons .= '</svg>';
  $switch_icons .= '<svg class="' . $icon_class . ' ' . $icon_class . '--collapse">';
  $switch_icons .= '<use xlink:href="/' . $icon_pack . '#collapse"></use>';
  $switch_icons .= '</svg>';

  $delete_icon = '<svg class="' . $icon_class . ' ' . $icon_class . '--delete">';
  $delete_icon .= '<use xlink:href="/' . $icon_pack . '#delete"></use>';
  $delete_icon .= '</svg>';

  $new_element = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'element-invisible',
        _paragraphs_extra_class('pretty-paragraphs', 'edit-toolbar'),
      ),
    ),
  );

  // Add a pretty collapse / edit button.
  $new_element['switch'] = array(
    '#type' => 'link',
    '#title' => $switch_icons . '<span class="' . $mobile_only . '">Edit</span>',
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--switch',
        $button_class . '--edit',
      ),
    ),
  );

  // Add a pretty delete button.
  $new_element['delete'] = array(
    '#type' => 'link',
    '#title' => $delete_icon . '<span class="' . $mobile_only . '">Delete</span>',
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
      'attributes' => array(
        'title' => t('Delete'),
      ),
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--delete',
      ),
    ),
  );

  // Allows us to persist whether a given paragraph is in 'edit' or
  // 'preview' mode.
  $new_element['edit_mode'] = array(
    '#type' => 'hidden',
    '#default_value' => 'preview',
    '#attributes' => array(
      'class' => array(
        _paragraphs_extra_class('pretty-paragraphs', 'edit-mode'),
      ),
    ),
  );

  $element['actions']['paragraphs_extra']['pretty_paragraphs']['edit_toolbar'] = $new_element;
}

/**
 * Attaches a rendered preview section to a paragraph item form.
 *
 * @param array &$element
 *   The paragraph item element to attach to.
 * @param string $viewmode
 *   The viewmode to attach the preview in.
 */
function paragraphs_extra_pretty_paragraphs_attach_preview(&$element, $viewmode) {
  $new_element = array(
    'paragraph_preview' => array(
      '#type' => 'container',
    ),
  );

  // Create a preview element.
  $preview_id = _paragraphs_extra_id('pretty_paragraphs', 'preview');
  if (get_class($element['#entity']) == 'ParagraphsItemEntity') {
    $new_element['paragraph_preview']['#attributes']['id'] = $preview_id;
    $new_element['paragraph_preview']['#attributes']['class'][] = _paragraphs_extra_class('pretty-paragraphs', 'edit-preview');
    $new_element['paragraph_preview']['preview'] = $element['#entity']->view($viewmode);
  }

  // This will be stripped on page-load in javascript. It exists here to
  // support a less-clunky javascript-disabled editing experience.
  $new_element['paragraph_preview']['preview']['#attributes']['class'][] = 'element-invisible';

  // Add class to identify input fields that can be hidden.
  foreach (element_children($element) as $key) {
    if ($key != 'paragraph_bundle_title' && $key != 'actions' && $key != 'paragraphs_extra' && $key != 'access_info') {
      $element[$key]['#attributes']['class'][] = _paragraphs_extra_class('pretty-paragraphs', 'edit-field');
    }
  }

  // Attach the preview element.
  $element['paragraphs_extra']['pretty_paragraphs'] += $new_element;

  // Add a button to trigger a paragraph item preview regeneration.
  $element['actions']['paragraphs_extra']['pretty_paragraphs']['preview_refresh'] = array(
    '#type' => 'submit',
    '#delta' => $element['#delta'],
    '#value' => t('Preview Item @delta', array('@delta' => $element['#delta'] + 1)),
    '#limit_validation_errors' => array(),
    '#submit' => array('paragraphs_extra_pretty_paragraphs_preview_rebuild_submit'),
    '#attributes' => array(
      'class' => array(
        'element-invisible',
        _paragraphs_extra_class('pretty-paragraphs', 'preview-refresh'),
      ),
    ),
    '#ajax' => array(
      'callback' => 'paragraphs_extra_pretty_paragraphs_preview_ajax',
      'wrapper' => $preview_id,
      'effect' => 'fade',
    ),
  );
}

/**
 * Attaches a toolbar for changing the preview view mode to a field widget.
 *
 * @param string $wrapper_id
 *   The wdget form wrapper id.
 * @param array &$element
 *   The paragraph field form to attach to.
 * @param array $form_state
 *   The form_state for the widget form to pull the current viewmode selection
 *   from.
 *
 * @return string
 *   The system name of the currently selected viewmode.
 */
function paragraphs_extra_pretty_paragraphs_attach_viewmode_toolbar($wrapper_id, &$element, $form_state) {
  if (isset($form_state['paragraphs_extra']['pretty_paragraphs']['viewmode'])) {
    $viewmode = $form_state['paragraphs_extra']['pretty_paragraphs']['viewmode'];
    unset($form_state['paragraphs_extra']['pretty_paragraphs']['viewmode']);
  }
  else {
    $viewmode = 'paragraphs_editor_preview';
  }

  if ($viewmode == 'default') {
    $viewmode = '';
  }

  $new_element = array(
    'viewmode_toolbar' => array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'element-invisible',
          _paragraphs_extra_class('pretty-paragraphs', 'viewmode-toolbar'),
        ),
      ),
    ),
  );

  $icon_classes = _paragraphs_extra_class('pretty-paragraphs', 'icon');
  $icon_classes .= ' ' . $icon_classes;
  $button_class = _paragraphs_extra_class('pretty-paragraphs', 'button');

  // Load SVG icons for the admin interface.
  $icon_pack = drupal_get_path('module', 'paragraphs_extra') . '/images/pretty_paragraphs_icons.svg';

  $full_icon = '<svg class="' . $icon_classes . '--full">';
  $full_icon .= '<use xlink:href="/' . $icon_pack . '#full"></use>';
  $full_icon .= '</svg>';

  $condensed_icon = '<svg class="' . $icon_classes . '--condensed">';
  $condensed_icon .= '<use xlink:href="/' . $icon_pack . '#condensed"></use>';
  $condensed_icon .= '</svg>';

  $new_element['viewmode_toolbar']['label'] = array(
    '#markup' => '<strong>' . t('Preview Mode') . '</strong>',
  );

  $new_element['viewmode_toolbar']['condensed'] = array(
    '#type' => 'link',
    '#title' => $condensed_icon,
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
      'attributes' => array(
        'title' => t('Teaser'),
      ),
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--push',
      ),
      _paragraphs_extra_data('pretty-paragraphs', 'viewmode') => 'paragraphs_editor_preview',
    ),
  );
  $new_element['viewmode_toolbar']['full'] = array(
    '#type' => 'link',
    '#title' => $full_icon,
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
      'attributes' => array(
        'title' => t('Full'),
      ),
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--push',
      ),
      _paragraphs_extra_data('pretty-paragraphs', 'viewmode') => 'default',
    ),
  );

  $new_element['viewmode_toolbar']['viewmode'] = array(
    '#type' => 'hidden',
    '#default_value' => $viewmode,
    '#attributes' => array(
      'class' => array(
        _paragraphs_extra_class('pretty-paragraphs', 'viewmode'),
      ),
    ),
  );

  $new_element['viewmode_toolbar']['rebuild'] = array(
    '#type' => 'submit',
    '#value' => 'Switch View Mode',
    '#limit_validation_errors' => array(),
    '#submit' => array('paragraphs_extra_pretty_paragraphs_reload_submit'),
    '#ajax' => array(
      'callback' => 'paragraphs_extra_pretty_paragraphs_reload_ajax',
      'wrapper' => $wrapper_id,
      'effect' => 'fade',
      'progress' => array(
        'message' => '',
      ),
    ),
    '#attributes' => array(
      'class' => array(
        'element-invisible',
        _paragraphs_extra_class('pretty-paragraphs', 'viewmode-refresh'),
      ),
    ),
  );

  $element['add_more']['paragraphs_extra']['pretty_paragraphs'] += $new_element;

  return $viewmode;
}

/**
 * Attaches a paragraph insertion toolbar to a paragraph item.
 *
 * @param string &$element
 *   The form element to attach to.
 * @param array $context
 *   The form element context array.
 */
function paragraphs_extra_pretty_paragraphs_attach_insert_toolbar(&$element, $context) {
  $new_element = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        _paragraphs_extra_class('pretty-paragraphs', 'insert-toolbar'),
      ),
    ),
  );

  $icon_class = _paragraphs_extra_class('pretty-paragraphs', 'icon');
  $button_class = _paragraphs_extra_class('pretty-paragraphs', 'button');

  // Load SVG icons for the admin interface.
  $icon_pack = drupal_get_path('module', 'paragraphs_extra') . '/images/pretty_paragraphs_icons.svg';

  $add_icon = '<svg class="' . $icon_class . ' ' . $icon_class . '--add">';
  $add_icon .= '<use xlink:href="/' . $icon_pack . '#add"></use>';
  $add_icon .= '</svg>';

  $new_element['add'] = array(
    '#type' => 'link',
    '#title' => $add_icon,
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
      'attributes' => array(
        'title' => t('Insert'),
      ),
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--insert',
      ),
    ),
  );

  $element['actions']['paragraphs_extra']['pretty_paragraphs']['insert_toolbar'] = $new_element;
}

/**
 * Replaces the default add mechanism with a beautified button.
 *
 * @param array &$element
 *   The form element to attach the button to.
 * @param array $context
 *   The widget form context argument.
 */
function paragraphs_extra_pretty_paragraphs_attach_add_button(&$element, $context) {
  $element['add_more']['type']['#access'] = FALSE;
  $element['add_more']['add_more']['#access'] = FALSE;

  foreach (element_children($element['add_more']) as $key) {
    if (isset($element['add_more'][$key]['#attributes']['class'])) {
      if (in_array('paragraphs-add-more-submit', $element['add_more'][$key]['#attributes']['class'])) {
        $element['add_more'][$key]['#access'] = FALSE;
      }
    }
  }

  $icon_class = _paragraphs_extra_class('pretty-paragraphs', 'icon');
  $button_class = _paragraphs_extra_class('pretty-paragraphs', 'button');

  // Load SVG icons for the admin interface.
  $icon_pack = drupal_get_path('module', 'paragraphs_extra') . '/images/pretty_paragraphs_icons.svg';

  $add_icon = '<svg class="' . $icon_class . ' ' . $icon_class . '--add">';
  $add_icon .= '<use xlink:href="/' . $icon_pack . '#add"></use>';
  $add_icon .= '</svg>';

  $new_element = array(
    '#type' => 'link',
    '#title' => $add_icon . '<span class="' . _paragraphs_extra_class('pretty-paragraphs', 'button-label') . '">' . t('Add Section') . '</span>',
    '#href' => '',
    '#options' => array(
      'html' => TRUE,
      'attributes' => array(
        'title' => t('Insert'),
      ),
    ),
    '#attributes' => array(
      'class' => array(
        $button_class,
        $button_class . '--add',
      ),
    ),
  );

  $element['add_more']['paragraphs_extra']['pretty_paragraphs']['add_more'] = $new_element;
}

/**
 * Refreshes the teaser for a paragraph item in the widget form.
 *
 * @param array $form
 *   The form the preview belongs to.
 * @param array &$form_state
 *   The form_state for the form the preview belongs to.
 */
function paragraphs_extra_pretty_paragraphs_preview_ajax($form, &$form_state) {
  $button = $form_state['triggering_element'];
  $delta = $button['#delta'];

  // Get information about the entity being refreshed.
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -4));
  $field_name = $element['#field_name'];
  $langcode = $element['#language'];
  $parents = $element['#field_parents'];

  // Update the entity structure with values from the fields.
  $field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);
  $paragraph_item = $field_state['entity'][$delta];

  $values = drupal_array_get_nested_value($form_state['input'], array_slice($button['#array_parents'], 0, -5));
  if (isset($values['add_more']['paragraphs_extra']['pretty_paragraphs']['viewmode_toolbar']['viewmode'])) {
    $viewmode = $values['add_more']['paragraphs_extra']['pretty_paragraphs']['viewmode_toolbar']['viewmode'];
  }
  else {
    $viewmode = 'paragraphs_editor_preview';
  }

  // Rebuild the preview.
  $preview = $paragraph_item->view($viewmode);
  $element['paragraphs_extra']['pretty_paragraphs']['paragraph_preview']['preview'] = $preview;

  return $element['paragraphs_extra']['pretty_paragraphs']['paragraph_preview'];
}

/**
 * Handles 'reload' submissions by forcing a form rebuild with updated viewmode.
 *
 * @param array $form
 *   The form to be rebuilt.
 * @param array &$form_state
 *   The form_state for the form to be rebuilt.
 */
function paragraphs_extra_pretty_paragraphs_handle_reload_submit($form, &$form_state) {
  $button = $form_state['triggering_element'];
  $values = drupal_array_get_nested_value($form_state['input'], array_slice($button['#array_parents'], 0, -1));
  if (isset($values['viewmode'])) {
    $form_state['paragraphs_extra']['pretty_paragraphs']['viewmode'] = $values['viewmode'];
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Delivers an the updated paragraph field widget form after a form rebuild.
 *
 * @param array $form
 *   The form containing the paragraphs field.
 * @param array &$form_state
 *   The form_state for the form containing the paragraphs field.
 */
function paragraphs_extra_pretty_paragraphs_reload_ajax($form, &$form_state) {
  $button = $form_state['triggering_element'];
  return drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -5));
}

/**
 * Attaches the Pretty Paragraphs settings to a field instance settings form.
 *
 * @param array &$form
 *   The field instance settings form.
 * @param array &$form_state
 *   The form_state for the field instance settings form.
 */
function paragraphs_extra_pretty_paragraphs_attach_instance_settings(&$form, &$form_state) {
  $settings = _paragraphs_extra_pretty_paragraphs_instance_settings($form_state['build_info']['args'][0]);
  $wrapper_id = _paragraphs_extra_id('pretty_paragraphs', 'options');
  $form['instance']['settings']['paragraphs_extra']['behaviors']['pretty_paragraphs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pretty Paragraphs'),
    '#weight' => -10,
    '#description' => t('Select which Pretty Paragraphs features you wish to enable. Note that the Default edit mode must be set to open.'),
    '#attributes' => array(
      'id' => array($wrapper_id),
    ),
    'click_to_edit' => array(
      '#type' => 'checkbox',
      '#title' => t('Click-to-Edit'),
      '#description' => t('Paragraphs will be toggled between preview mode and edit mode on click.'),
      '#default_value' => $settings['click_to_edit'],
      '#attributes' => array(
        'class' => array('click-to-edit'),
      ),
      '#states' => array(
        'visible' => array(
          '#edit-instance-settings-default-edit-mode' => array('value' => 'open'),
        ),
      ),
    ),
    'preview_modes' => array(
      '#type' => 'checkbox',
      '#title' => t('Preview Modes'),
      '#description' => t('A toolbar will be displayed to switch between preview and full view modes.'),
      '#default_value' => $settings['preview_modes'],
      '#states' => array(
        'visible' => array(
          '#' . $wrapper_id . ' .click-to-edit'  => array('checked' => TRUE),
          '#edit-instance-settings-default-edit-mode' => array('value' => 'open'),
        ),
      ),
    ),
    'modal_select' => array(
      '#type' => 'checkbox',
      '#title' => t('Modal Select'),
      '#description' => t('Replaces the default insert methods with a views-based selection modal.'),
      '#default_value' => $settings['modal_select'],
      '#attributes' => array(
        'class' => array('modal-select'),
      ),
      '#states' => array(
        'visible' => array(
          '#edit-instance-settings-default-edit-mode' => array('value' => 'open'),
        ),
      ),
    ),
    'insert_between' => array(
      '#type' => 'checkbox',
      '#title' => t('Insert Between'),
      '#description' => t('Puts an insert button between sections.'),
      '#default_value' => $settings['insert_between'],
      '#states' => array(
        'visible' => array(
          '#' . $wrapper_id . ' .modal-select'  => array('checked' => TRUE),
          '#edit-instance-settings-default-edit-mode' => array('value' => 'open'),
        ),
      ),
    ),
  );
}

function _paragraphs_extra_pretty_paragraphs_instance_settings($instance) {
  $settings = _paragraphs_extra_instance_settings($instance, 'pretty_paragraphs', array(
    'click_to_edit' => FALSE,
    'modal_select' => FALSE,
    'preview_modes' => FALSE,
    'insert_between' => FALSE,
  ));

  if ($instance['settings']['default_edit_mode'] == 'open') {

    if (!$settings['modal_select']) {
      $settings['insert_between'] = FALSE;
    }

    if (!$settings['click_to_edit']) {
      $settings['preview_modes'] = FALSE;
    }
  }
  else {
    foreach ($required_keys as $key) {
      $settings[$key] = NULL;
    }
  }

  return $settings;
}
