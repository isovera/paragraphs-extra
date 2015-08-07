<?php
/**
 * @file
 * Provides an enriched paragraphs bundle selection experience.
 */

/**
 * Adds the CTools modal for bundle selection to a form element.
 *
 * @param array &$element
 *   The form element to attach to.
 * @param array $context
 *   The context array for the form element to attach to.
 */
function paragraphs_extra_bundle_info_modal_attach(&$element, $context) {
  $attached = &drupal_static(__FUNCTION__);

  if (!$attached) {
    $element['#attached']['js'][] = drupal_get_path('module', 'paragraphs_extra') . '/js/BundleSelect.js';

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_add_js();

    $classes = array(
      'element-invisible',
      _paragraphs_extra_class('bundle-info', 'modal-trigger'),
    );
    $new_element = array(
      '#markup' => ctools_modal_text_button(
        t('Select Bundle'),
        'admin/paragraphs-extra/nojs/bundle-select/' . _paragraphs_extra_ajax_api_instance_id($context),
        t('Select Bundle'),
        implode(' ', $classes)
      ),
    );

    $element['add_more']['paragraphs_extra']['bundle_info']['modal'] = $new_element;
    $attached = TRUE;
  }
}

/**
 * Handle when a bundle is deleted.
 *
 * @param object $bundle
 *   The bundle object being deleted.
 */
function paragraphs_extra_bundle_info_handle_delete($bundle) {
  $bundle_info = paragraphs_extra_bundle_info_load($bundle);
  if ($bundle_info->icon) {
    $file = file_load($bundle_info->icon);
    if ($file) {
      file_delete($file, TRUE);
    }
  }
}

/**
 * Attaches extra bundle settings to the bundle info form.
 *
 * @param array &$form
 *   The form to attach to.
 * @param array &$form_state
 *   The form state for the form to attach to.
 */
function paragraphs_extra_bundle_info_form_attach(&$form, &$form_state) {
  if (!empty($form_state['build_info']['args'])) {
    $bundle_info = paragraphs_extra_bundle_info_load($form_state['build_info']['args'][0]->bundle);
  }
  else {
    $bundle_info = NULL;
  }

  if (!$bundle_info) {
    $bundle_info = new stdClass();
  }

  $form['paragraphs_extra']['#tree'] = TRUE;

  $form['paragraphs_extra']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('A description to help users select a bundle.'),
    '#default_value' => isset($bundle_info->description) ? $bundle_info->description : '',
  );

  $form['paragraphs_extra']['icon'] = array(
    '#type' => 'managed_file',
    '#title' => t('Icon'),
    '#description' => t('An icon to show for bundle selection.'),
    '#default_value' => isset($bundle_info->icon) ? $bundle_info->icon : '',
    '#upload_location' => 'public://',
  );

  $form['paragraphs_extra']['old_icon'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($bundle_info->icon) ? $bundle_info->icon : '',
  );

  $form['#submit'][] = 'paragraphs_extra_bundle_info_form_submit';
}

/**
 * Handles submissions for the bundle info form.
 *
 * @param array $form
 *   The form being submitted.
 * @param array &$form_state
 *   The form state for the form being submitted.
 */
function paragraphs_extra_bundle_info_handle_form_submit($form, &$form_state) {
  if (!empty($form_state['build_info']['args'])) {
    $bundle = $form_state['build_info']['args'][0]->bundle;
  }
  else {
    $bundle = $form_state['values']['bundle'];
  }

  $bundle_info = paragraphs_extra_bundle_info_load($bundle);
  if (!$bundle_info) {
    $bundle_info = new stdClass();
    $bundle_info->is_new = TRUE;
  }

  // Update basic field information.
  $bundle_info->bundle = $bundle;
  $bundle_info->description = $form_state['values']['paragraphs_extra']['description'];

  // Save new icon.
  $icon = $form_state['values']['paragraphs_extra']['icon'];
  $file = file_load($icon);
  $file->status = FILE_STATUS_PERMANENT;
  file_save($file);
  file_usage_add($file, 'paragraphs_extra', 'bundle', $file->fid);
  $bundle_info->icon = $form_state['values']['paragraphs_extra']['icon'];

  // Remove old icon if necessary.
  $old_icon = $form_state['values']['paragraphs_extra']['old_icon'];
  if ($old_icon && $old_icon != $bundle_info->icon) {
    $file = file_load($old_icon);
    if ($file) {
      file_delete($file, TRUE);
    }
  }

  paragraphs_extra_bundle_info_save($bundle, $bundle_info);
}

/**
 * Load additional fields for the paragraphs bundle.
 *
 * @param object $bundle
 *   The bundle to load info about.
 *
 * @return object
 *   The extra bundle info object associated with a bundle.
 */
function paragraphs_extra_bundle_info_load($bundle) {
  return db_select('paragraphs_extra_bundle_info', 'info')
    ->fields('info')
    ->condition('info.bundle', $bundle)
    ->execute()
    ->fetchObject();
}

/**
 * Saves additional fields for the paragraphs bundle.
 *
 * @param object $bundle
 *   The bundle object to save the info for.
 * @param object $info
 *   The info object to be saved.
 */
function paragraphs_extra_bundle_info_save($bundle, $info) {
  if (isset($info->is_new) && $info->is_new) {
    drupal_write_record('paragraphs_extra_bundle_info', $info);
  }
  else {
    drupal_write_record('paragraphs_extra_bundle_info', $info, array('id', 'bundle'));
  }
}

/**
 * A page callback for providing the ctools bundle selection modal.
 *
 * @param bool $js
 *   Whether or not javascript is enabled.
 * @param string $instance_id
 *   The field instance id being requested.
 *
 * @return mixed
 *   The rendered page as a string or access denied.
 */
function paragraphs_extra_bundle_info_select_page($js, $instance_id) {

  if ($js) {
    $implementations = module_implements('paragraphs_extra_bundle_info_page');
    if (!empty($implementations)) {
      foreach ($implementations as $module) {
        $function = $module . '_paragraphs_extra_bundle_info_page';
        $output = $function($instance_id);
      }
    }
    else {
      $output = views_embed_view('paragraphs_extra_bundles');
    }

    drupal_alter('paragraphs_extra_bundle_info_page', $output, $insatnce_id);

    ctools_include('ajax');
    ctools_include('modal');
    ctools_modal_render(t('Insert'), $output);
  }
  else {
    return MENU_ACCESS_DENIED;
  }
}
